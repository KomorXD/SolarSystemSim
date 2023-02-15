#include "DebugScene.hpp"
#include "../Logger.hpp"
#include "../Event.hpp"

#include <imgui/imgui.h>

DebugScene::DebugScene()
{
	LOG_INFO("DebugScene initialized.");
}

DebugScene::~DebugScene()
{
	LOG_INFO("DebugScene destroyed.");
}

void DebugScene::OnEvent(Event& ev)
{
	if(ev.Type == Event::KeyPressed && ev.Key.Code == Key::A)
	{
		m_ShouldBeRendered = !m_ShouldBeRendered;

		LOG_INFO("Event caught, flag is now: {}", m_ShouldBeRendered ? "true" : "false");
	}
}

void DebugScene::OnInput()
{

}

void DebugScene::OnUpdate(float ts)
{
	m_TimePassedInSeconds += ts;

	if (m_TimePassedInSeconds > 1.0f)
	{
		m_TimePassedInSeconds = 0.0f;

		LOG_INFO("1 second passed, FPS: {}", 1.0f / ts);
		LOG_WARN("Mouse position: {}x{}", Input::GetMousePosition().x, Input::GetMousePosition().y);
	}
}

void DebugScene::OnRender()
{
	if (!m_ShouldBeRendered)
	{
		return;
	}

	ImGui::Begin("DebugWindow");
	ImGui::SetWindowFontScale(2.0f);
	
	if (ImGui::Button("Click :3"))
	{
		LOG_INFO("DebugWindow button clicked.");
	}

	ImGui::End();
}
