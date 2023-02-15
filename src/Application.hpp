#pragma once

#include "Event.hpp"

#include <string>
#include <memory>

struct GLFWwindow;
class Scene;

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

	inline GLFWwindow* GetWindow()	  const { return m_Window; }
	inline WindowSpec GetWindowSpec() const { return m_WindowSpec; }

	inline static Application* GetInstance() { return s_Instance; }

private:
	void SetWindowCallbacks();

	std::unique_ptr<Scene> m_CurrentScene;

	GLFWwindow* m_Window = nullptr;
	WindowSpec  m_WindowSpec;

	EventQueue m_EventQueue;

	inline static Application* s_Instance = nullptr;
};