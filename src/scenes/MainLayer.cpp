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
	if (ev.Type == Event::WindowResized)
	{
		ev.Size.Width = (uint32_t)(ev.Size.Width * 0.66f);
		m_Scene->OnEvent(ev);

		return;
	}

	if (m_IsViewportFocused)
	{
		m_Scene->OnEvent(ev);
	}
}

void MainLayer::OnInput()
{
	if (m_IsViewportFocused)
	{
		m_Scene->OnInput();
	}
}

void MainLayer::OnUpdate(float ts)
{
	if (m_IsViewportFocused)
	{
		m_Scene->OnUpdate(ts);
	}
}

void MainLayer::OnImGuiRender()
{
	WindowSpec windowSpec = Application::GetInstance()->GetWindowSpec();

	m_Scene->OnRender();

	ImGui::SetNextWindowPos({ 0.0f, 0.0f });
	ImGui::SetNextWindowSize({ windowSpec.Width * 0.66f, windowSpec.Height * 1.0f });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
	ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	m_IsViewportFocused = ImGui::IsWindowHovered();
	ImGui::Image((ImTextureID)m_Scene->GetFramebufferTextureID(), ImGui::GetContentRegionAvail(), { 0.0f, 1.0f }, { 1.0f, 0.0f });

	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::SetNextWindowPos({ windowSpec.Width * 0.66f, 0.0f });
	ImGui::SetNextWindowSize({ windowSpec.Width * 0.34f, windowSpec.Height * 1.0f });
	m_Scene->OnConfigRender();
}
