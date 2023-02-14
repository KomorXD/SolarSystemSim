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

	glfwSetErrorCallback(
		[](int32_t errorCode, const char* description)
		{
			LOG_ERROR("GLFW error #{}: {}", errorCode, description);
		});

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

		// OnUpdate
		// OnInput
		// OnRender

		ImGui::Render();
		
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}
}
