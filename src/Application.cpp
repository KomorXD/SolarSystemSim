#include "Application.hpp"
#include "Logger.hpp"
#include "layers/EditorLayer.hpp"
#include "OpenGL.hpp"
#include "TriggerClock.hpp"
#include "TextureManager.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <imgui/ImGuizmo.h>

#include <mutex>
#include <thread>

Application::Application(const WindowSpec& spec)
	: m_WindowSpec(spec)
{
	Logger::Init();

	LOG_INFO("Logger initialized");

	if (glfwInit() == GLFW_FALSE)
	{
		LOG_CRITICAL("Failed to initialize GLFW!");

		return;
	}

	LOG_INFO("GLFW initialized");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	m_Window = glfwCreateWindow(m_WindowSpec.Width, m_WindowSpec.Height,
		m_WindowSpec.Title.c_str(), nullptr, nullptr);

	int width{};
	int height{};

	glfwGetWindowSize(m_Window, &width, &height);

	m_WindowSpec.Width = width;
	m_WindowSpec.Height = height;

	if (!m_Window)
	{
		LOG_CRITICAL("Failed to create a window!");

		return;
	}

	LOG_INFO("Window created");

	glfwMakeContextCurrent(m_Window);
	glfwSwapInterval(0);

	glfwSetWindowUserPointer(m_Window, this);
	SetWindowCallbacks();

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		LOG_CRITICAL("Failed to load GLAD!");

		return;
	}

	LOG_INFO("GLAD loaded");

	TextureManager::Init();

	s_Instance = this;
}

Application::~Application()
{
	if (m_Window)
	{
		glfwDestroyWindow(m_Window);
	}
	
	glfwTerminate();
}

void Application::Run()
{
	if (!s_Instance)
	{
		LOG_ERROR("Application loop was run without a valid Application instance!");

		return;
	}

	m_CurrentLayer = std::make_unique<EditorLayer>();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();

	io.Fonts->AddFontDefault();

	ImFont* font = io.Fonts->AddFontFromFileTTF("res/fonts/tahoma.ttf", 16.0f);
	IM_ASSERT(font != nullptr);

	ApplyImGuiStyles();

	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
	ImGui_ImplOpenGL3_Init();

	double prevTime = 0.0f;
	double currTime = 0.0f;
	double timestep = 1.0f / 60.0f;
	uint32_t tpsFired = 0;
	std::mutex layerMtx;

	TriggerClock tickClock([&]() { 
		std::lock_guard<std::mutex> lg(layerMtx);
		m_CurrentLayer->OnTick(); tpsFired++;
	});
	tickClock.SetInterval(TPS_STEP * 1000);
	tickClock.Start();

	TriggerClock fpsClock([&]() { LOG_INFO("Timestep: {:.5f}ms", timestep); });
	fpsClock.SetInterval(1000);
	// fpsClock.Start();

	TriggerClock tpsClock([&]() { LOG_INFO("OnTick fired {} times last second", tpsFired); tpsFired = 0; });
	tpsClock.SetInterval(1000);
	// tpsClock.Start();

	std::thread clockThread(&Application::UpdateClocksWorker, this);

	while (!glfwWindowShouldClose(m_Window))
	{
		prevTime = currTime;
		currTime = glfwGetTime();
		timestep = currTime - prevTime;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
		ImGui::PushFont(font);

		Event ev{};

		{
			std::lock_guard<std::mutex> lg(layerMtx);
			while (m_EventQueue.PollEvents(ev))
			{
				m_CurrentLayer->OnEvent(ev);
			}

			m_CurrentLayer->OnInput();
			m_CurrentLayer->OnUpdate((float)timestep);

			m_CurrentLayer->OnImGuiRender();
		}

		ImGui::PopFont();
		ImGui::Render();
		
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	clockThread.detach();
}

void Application::CloseApplication()
{
	glfwSetWindowShouldClose(m_Window, (int)true);
}

void Application::SetWindowCallbacks()
{
	glfwSetErrorCallback(
		[](int errorCode, const char* description)
		{
			LOG_ERROR("GLFW error #{}: {}", errorCode, description);
		});

	glfwSetKeyCallback(m_Window,
		[](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			Application* app = (Application*)glfwGetWindowUserPointer(window);

			Event ev{};

			switch (action)
			{
			case GLFW_PRESS:   ev.Type = Event::KeyPressed;  break;
			case GLFW_RELEASE: ev.Type = Event::KeyReleased; break;
			case GLFW_REPEAT:  ev.Type = Event::KeyHeld;	 break;
			}

			ev.Key.Code = (Key)key;
			ev.Key.AltPressed = Input::IsKeyPressed((Key)GLFW_KEY_LEFT_ALT);
			ev.Key.CtrlPressed = Input::IsKeyPressed((Key)GLFW_KEY_LEFT_CONTROL);
			ev.Key.ShiftPressed = Input::IsKeyPressed((Key)GLFW_KEY_LEFT_SHIFT);

			app->m_EventQueue.SubmitEvent(ev);
		});

	glfwSetCursorPosCallback(m_Window,
		[](GLFWwindow* window, double xPos, double yPos)
		{
			Application* app = (Application*)glfwGetWindowUserPointer(window);

			Event ev{};

			ev.Type = Event::MouseMoved;
			ev.Mouse.PosX = (float)xPos;
			ev.Mouse.PosY = (float)yPos;

			app->m_EventQueue.SubmitEvent(ev);
		});

	glfwSetMouseButtonCallback(m_Window,
		[](GLFWwindow* window, int button, int action, int mods)
		{
			Application* app = (Application*)glfwGetWindowUserPointer(window);

			Event ev{};

			ev.Type = (action == GLFW_RELEASE ? Event::MouseButtonReleased : Event::MouseButtonPressed);
			ev.MouseButton.Button = (MouseButton)button;

			app->m_EventQueue.SubmitEvent(ev);
		});

	glfwSetScrollCallback(m_Window,
		[](GLFWwindow* window, double xOffset, double yOffset)
		{
			Application* app = (Application*)glfwGetWindowUserPointer(window);

			Event ev{};

			ev.Type = Event::MouseWheelScrolled;
			ev.MouseWheel.OffsetX = (float)xOffset;
			ev.MouseWheel.OffsetY = (float)yOffset;

			app->m_EventQueue.SubmitEvent(ev);
		});

	glfwSetWindowSizeCallback(m_Window,
		[](GLFWwindow* window, int width, int height)
		{
			Application* app = (Application*)glfwGetWindowUserPointer(window);

			Event ev{};

			ev.Type = Event::WindowResized;
			ev.Size.Width = width;
			ev.Size.Height = height;

			app->m_WindowSpec.Width = width;
			app->m_WindowSpec.Height = height;

			app->m_EventQueue.SubmitEvent(ev);
		});
}

void Application::UpdateClocksWorker()
{
	while (true)
	{
		TriggerClock::UpdateClocks();
	}
}

void Application::ApplyImGuiStyles()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
	style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}
