#pragma once

#include <string>

struct GLFWwindow;

class Application
{
public:
	struct WindowSpec
	{
		int32_t Width	  = 1280;
		int32_t Height	  = 720;
		std::string Title = "Solar System Simulator";
	};

	Application(const WindowSpec& spec = {});
	~Application();

	void Run();

	inline Application* GetInstance() const { return s_Instance; }
	inline WindowSpec GetWindowSpec() const { return m_WindowSpec; }

private:
	GLFWwindow* m_Window = nullptr;
	WindowSpec  m_WindowSpec;

	inline static Application* s_Instance = nullptr;
};