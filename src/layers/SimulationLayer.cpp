#include "SimulationLayer.hpp"
#include "../Application.hpp"

#include <imgui/imgui.h>

SimulationLayer::SimulationLayer()
{
	// m_ActiveScene = std::make_unique<dupa>(?);
}

void SimulationLayer::OnEvent(Event& ev)
{
	if (ev.Type == Event::KeyPressed && ev.Key.Code == Key::Escape)
	{
		Application::GetInstance()->CloseApplication();
	}
}

void SimulationLayer::OnInput()
{
}

void SimulationLayer::OnUpdate(float ts)
{
}

void SimulationLayer::OnImGuiRender()
{
	ImGui::Begin("tedsdtds");
	ImGui::Text("siema");
	ImGui::End();
}
