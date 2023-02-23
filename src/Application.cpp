#include "Application.hpp"
#include "Logger.hpp"
#include "layers/EditorLayer.hpp"
#include "OpenGL.hpp"

#include <glad/glad.h>
#include <glfw/glfw3.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <imgui/ImGuizmo.h>

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
	glfwSwapInterval(1);

	glfwSetWindowUserPointer(m_Window, this);
	SetWindowCallbacks();

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		LOG_CRITICAL("Failed to load GLAD!");

		return;
	}

	LOG_INFO("GLAD loaded");

	s_Instance = this;
}

Application::~Application()
{
	if (m_Window)
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}
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

	ImFont* font = io.Fonts->AddFontFromFileTTF("res/fonts/tahoma.ttf", 20.0f);
	IM_ASSERT(font != nullptr);

	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
	ImGui_ImplOpenGL3_Init();

	double prevTime = 0.0f;
	double currTime = 0.0f;
	double timestep = 1.0f / 60.0f;

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
		
		while(m_EventQueue.PollEvents(ev))
		{
			m_CurrentLayer->OnEvent(ev);
		}

		m_CurrentLayer->OnInput();
		m_CurrentLayer->OnUpdate((float)timestep);
		m_CurrentLayer->OnImGuiRender();

		ImGui::PopFont();
		ImGui::Render();
		
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}
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

			ev.Type = action == GLFW_RELEASE ? Event::MouseButtonReleased : Event::MouseButtonPressed;
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
