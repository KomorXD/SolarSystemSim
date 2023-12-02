#pragma once

#include "Event.hpp"

#include <string>
#include <memory>
#include <stack>

struct GLFWwindow;
class Layer;

struct WindowSpec
{
	int32_t Width = 1280;
	int32_t Height = 720;
	std::string Title = "Solar System Simulator";
};

class Application
{
public:
	Application(const WindowSpec& spec = {});
	~Application();

	void Run();
	void CloseApplication();

	void PushLayer(std::unique_ptr<Layer>&& layer);
	void PopLayer();

	inline GLFWwindow* GetWindow()	  const { return m_Window; }
	inline WindowSpec GetWindowSpec() const { return m_WindowSpec; }

	inline static Application* GetInstance() { return s_Instance; }
	
	inline static constexpr uint32_t TPS = 240;
	inline static constexpr float TPS_STEP = 1.0f / TPS;
	inline static float TPS_MULTIPLIER = 1.0f;

private:
	void SetWindowCallbacks();
	void UpdateClocksWorker();
	void ApplyImGuiStyles();

	std::stack<std::unique_ptr<Layer>> m_Layers;;

	GLFWwindow* m_Window = nullptr;
	WindowSpec  m_WindowSpec;

	EventQueue m_EventQueue;

	inline static Application* s_Instance = nullptr;
};