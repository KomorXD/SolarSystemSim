#include "DebugScene.hpp"
#include "../Logger.hpp"
#include "../Timer.hpp"
#include "../Event.hpp"
#include "../Application.hpp"
#include "../renderer/Renderer.hpp"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

DebugScene::DebugScene()
{
	FUNC_PROFILE();

	WindowSpec spec = Application::GetInstance()->GetWindowSpec();

	m_FB = std::make_unique<Framebuffer>();
	m_FB->AttachTexture(spec.Width, spec.Height);
	m_FB->AttachRenderBuffer(spec.Width, spec.Height);
	m_FB->UnbindBuffer();

	Renderer::Init();

	m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	m_Camera.SetViewportSize({ (float)spec.Width * 0.66f, (float)spec.Height });

	LOG_INFO("DebugScene initialized.");
}

DebugScene::~DebugScene()
{
	Renderer::Shutdown();

	LOG_INFO("DebugScene destroyed.");
}

void DebugScene::OnEvent(Event& ev)
{
	if (ev.Type == Event::KeyPressed && ev.Key.Code == Key::RightShift)
	{
		Renderer::ToggleWireframe();
	}

	m_Camera.OnEvent(ev);
}

void DebugScene::OnInput()
{
}

void DebugScene::OnUpdate(float ts)
{
	m_FPS = 1.0f / ts;

	m_Camera.OnUpdate(ts);
}

void DebugScene::OnRender()
{
	m_FB->BindBuffer();
	m_FB->BindRenderBuffer();

	Renderer::ClearColor(glm::vec4(0.05f, 0.05f, 0.05f, 1.0f));
	Renderer::Clear();

	Renderer::SceneBegin(m_Camera);
	Renderer::DrawSphere({ 2.0f, 0.0f, -3.0f }, 0.5f, m_SphereColor);
	Renderer::DrawSphere({ 0.0f, -0.5f, -6.0f }, 1.0f, { 1.0f, 1.0f, 0.0f, 1.0f });
	Renderer::DrawQuad({ -2.0f, 0.0f, -5.0f }, glm::vec3(0.75f), { 0.0f, 1.0f, 1.0f, 1.0f });
	Renderer::SceneEnd();

	m_FB->UnbindRenderBuffer();
	m_FB->UnbindBuffer();
	m_FB->BindTexture(1);
}

void DebugScene::OnConfigRender()
{
	glm::vec3 cameraPos = m_Camera.GetPosition();

	ImGui::Begin("Control panel", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	ImGui::Text("FPS: %.2f", m_FPS);
	ImGui::Separator();

	ImGui::Text("Camera position: [%.2f %.2f %.2f]", cameraPos.x, cameraPos.y, cameraPos.z);
	ImGui::Separator();

	ImGui::Text("Sphere color picker");
	ImGui::ColorEdit4("Color", glm::value_ptr(m_SphereColor));
	ImGui::Separator();

	ImGui::End();
}

uint32_t DebugScene::GetFramebufferTextureID() const
{
	return m_FB->GetTextureID();
}
