#include "Application.hpp"
#include "Logger.hpp"

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

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

	m_Window = glfwCreateWindow(m_WindowSpec.Width, m_WindowSpec.Height,
		m_WindowSpec.Title.c_str(), nullptr, nullptr);

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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
	ImGui_ImplOpenGL3_Init();

	while (!glfwWindowShouldClose(m_Window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.33f, 0.33f, 0.33f, 1.0f);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		// OnEvents
		// Event ev{};
		// 
		// while(m_EventQueue.PollEvents(ev))
		// {
		//	   m_CurrentScene->OnEvent(ev, ts);
		// }
		// 
		// OnUpdate
		// m_CurrentScene->OnUpdate(ts);
		// 
		// OnInput
		// m_CurretnScene->OnInput();
		// 
		// OnRender
		// m_CurrentScene->OnRender();

		ImGui::Render();
		
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}
}

void Application::SetWindowCallbacks()
{
	glfwSetErrorCallback(
		[](int32_t errorCode, const char* description)
		{
			LOG_ERROR("GLFW error #{}: {}", errorCode, description);
		});

	glfwSetKeyCallback(m_Window,
		[](GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
		{
			if (action != GLFW_PRESS)
			{
				return;
			}

			Application* app = (Application*)glfwGetWindowUserPointer(window);

			Event ev{};

			ev.Type = Event::KeyPressed;
			ev.Key.Code = (Key)key;
			ev.Key.AltPressed = Input::IsKeyPressed((Key)GLFW_KEY_LEFT_ALT);
			ev.Key.CtrlPressed = Input::IsKeyPressed((Key)GLFW_KEY_LEFT_CONTROL);
			ev.Key.ShiftPressed = Input::IsKeyPressed((Key)GLFW_KEY_LEFT_SHIFT);

			app->m_EventQueue.SubmitEvent(ev);
		});

	glfwSetMouseButtonCallback(m_Window,
		[](GLFWwindow* window, int32_t button, int32_t action, int32_t mods)
		{
			if (action != GLFW_PRESS)
			{
				return;
			}

			Application* app = (Application*)glfwGetWindowUserPointer(window);

			Event ev{};

			ev.Type = Event::MouseButtonPressed;
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
		[](GLFWwindow* window, int32_t width, int32_t height)
		{
			Application* app = (Application*)glfwGetWindowUserPointer(window);

			Event ev{};

			ev.Type = Event::WindowResized;
			ev.Size.Width = width;
			ev.Size.Height = height;

			app->m_EventQueue.SubmitEvent(ev);

			app->m_WindowSpec.Width = width;
			app->m_WindowSpec.Height = height;

			LOG_INFO("Window resized to {}x{}", app->GetWindowSpec().Width, app->GetWindowSpec().Height);
		});
}
