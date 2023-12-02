#include "SimulationLayer.hpp"
#include "../Application.hpp"
#include "../scenes/EditorScene.hpp"

#include <imgui/imgui.h>

SimulationLayer::SimulationLayer(std::unique_ptr<EditorScene>& scene)
{
	WindowSpec spec = Application::GetInstance()->GetWindowSpec();
	Event dummyEv{};
	dummyEv.Type = Event::WindowResized;
	dummyEv.Size.Width = spec.Width;
	dummyEv.Size.Height = spec.Height;

	m_Scene = std::make_unique<EditorScene>(*scene);
	m_Scene->OnEvent(dummyEv);
}

SimulationLayer::~SimulationLayer()
{
}

void SimulationLayer::OnEvent(Event& ev)
{
	if (ev.Type == Event::KeyPressed && ev.Key.Code == Key::Escape)
	{
		Application::GetInstance()->PopLayer();

		return;
	}
}

void SimulationLayer::OnInput()
{
	m_Scene->OnInput();
}

void SimulationLayer::OnUpdate(float ts)
{
	m_Scene->OnUpdate(ts);
}

void SimulationLayer::OnTick()
{
	m_Scene->OnTick();
}

void SimulationLayer::OnImGuiRender()
{
	WindowSpec windowSpec = Application::GetInstance()->GetWindowSpec();

	m_Scene->OnRender();

	ImGui::SetNextWindowPos({ 0.0f, 0.0f });
	ImGui::SetNextWindowSize({ (float)windowSpec.Width, (float)windowSpec.Height });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
	ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

	ImGui::Image((ImTextureID)m_Scene->GetFramebufferTextureID(), ImGui::GetContentRegionAvail(), { 0.0f, 1.0f }, { 1.0f, 0.0f });

	ImGui::End();
	ImGui::PopStyleVar();
}
