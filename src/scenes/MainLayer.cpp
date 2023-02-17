#include "MainLayer.hpp"
#include "DebugScene.hpp"
#include "../Application.hpp"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

MainLayer::MainLayer()
{
	m_Scene = std::make_unique<DebugScene>();
}

void MainLayer::OnEvent(Event& ev)
{
	m_Scene->OnEvent(ev);
}

void MainLayer::OnInput()
{
	m_Scene->OnInput();
}

void MainLayer::OnUpdate(float ts)
{
	m_Scene->OnUpdate(ts);
}

static bool s_RenderTriangle = true;

void MainLayer::OnImGuiRender()
{
	WindowSpec windowSpec = Application::GetInstance()->GetWindowSpec();

	m_Scene->OnRender();

	ImGui::SetNextWindowPos({ 0.0f, 0.0f });
	ImGui::SetNextWindowSize({ windowSpec.Width * 0.66f, windowSpec.Height * 1.0f });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
	ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	if (s_RenderTriangle)
	{
		ImGui::Image((ImTextureID)m_Scene->GetFramebufferTextureID(), ImGui::GetContentRegionAvail(), { 0.0f, 1.0f }, { 1.0f, 0.0f });
	}

	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::SetNextWindowPos({ windowSpec.Width * 0.66f, 0.0f });
	ImGui::SetNextWindowSize({ windowSpec.Width * 0.34f, windowSpec.Height * 1.0f });
	m_Scene->OnConfigRender();
}
