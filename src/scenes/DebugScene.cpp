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

	m_Camera.SetPosition(glm::vec3(0.0f, 3.0f, 0.0f));
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

	Renderer::ClearColor(glm::vec4(0.33f, 0.33f, 0.33f, 1.0f));
	Renderer::Clear();

	Renderer::SceneBegin(m_Camera);

	if (m_ShowGrid)
	{
		DrawGridPlane();
	}

	Renderer::DrawSphere({ 2.0f, 0.0f, -10.0f }, 0.5f, m_SphereColor);
	Renderer::DrawSphere({ 0.0f, -0.5f, -8.0f }, 1.0f, { 1.0f, 1.0f, 0.0f, 1.0f });
	Renderer::DrawQuad({ -2.0f, 0.0f, -7.0f }, glm::vec3(0.75f), { 0.0f, 1.0f, 1.0f, 1.0f });
	Renderer::SceneEnd();

	m_FB->UnbindRenderBuffer();
	m_FB->UnbindBuffer();
	m_FB->BindTexture(1);
}

void DebugScene::OnConfigRender()
{
	glm::vec3 cameraPos = m_Camera.GetPosition();

	ImGui::Begin("Control panel", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	ImGui::NewLine();
	ImGui::Text("FPS: %.2f", m_FPS);
	ImGui::Text("Camera position: [%.2f %.2f %.2f]", cameraPos.x, cameraPos.y, cameraPos.z);
	ImGui::NewLine();
	ImGui::Separator();
	ImGui::NewLine();

	ImGui::Text("Sphere color picker");
	ImGui::ColorEdit4("Color", glm::value_ptr(m_SphereColor));
	ImGui::NewLine();
	ImGui::Separator();
	ImGui::NewLine();

	ImGui::Checkbox("Show grid", &m_ShowGrid);
	
	if (ImGui::Button("Toggle wireframe"))
	{
		Renderer::ToggleWireframe();
	}

	ImGui::NewLine();
	ImGui::Separator();
	ImGui::NewLine();

	ImGui::End();
}

uint32_t DebugScene::GetFramebufferTextureID() const
{
	return m_FB->GetTextureID();
}

void DebugScene::DrawGridPlane()
{
	constexpr float distance = 30.0f;
	constexpr glm::vec3 lineColor(0.77f);

	Renderer::SetLineWidth(1.0f);

	for (float x = -distance; x <= distance; x += 1.0f)
	{
		Renderer::DrawLine({ x, 0.0f, -distance }, { x, 0.0f, distance }, glm::vec4(lineColor, 1.0f - (std::abs(x) / (distance))));
	}

	for (float z = -distance; z <= distance; z += 1.0f)
	{
		Renderer::DrawLine({ -distance, 0.0f, z }, { distance, 0.0f, z }, glm::vec4(lineColor, 1.0f - (std::abs(z) / (distance))));
	}
}
