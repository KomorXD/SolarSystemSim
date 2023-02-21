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
	m_FB->AttachTexture((uint32_t)(spec.Width * 0.66f), spec.Height);
	m_FB->AttachRenderBuffer((uint32_t)(spec.Width * 0.66f), spec.Height);
	m_FB->UnbindBuffer();

	Renderer::Init();
	Renderer::OnWindowResize((uint32_t)(spec.Width * 0.66f), spec.Height);

	m_Camera.SetPosition(glm::vec3(0.0f, 3.0f, 0.0f));
	m_Camera.SetViewportSize({ (float)spec.Width * 0.66f, (float)spec.Height });

	std::array<std::string, 6> faces =
	{
		"res/textures/skybox/right.png",
		"res/textures/skybox/left.png",
		"res/textures/skybox/top.png",
		"res/textures/skybox/bottom.png",
		"res/textures/skybox/front.png",
		"res/textures/skybox/back.png"
	};

	m_SkyboxTex = std::make_shared<Cubemap>(faces);

	float xd = 30.0f;

	for (float x = -xd; x <= xd; x += 5.0f)
	{
		for (float z = -xd; z <= xd; z += 5.0f)
		{
			m_Planets.emplace_back(glm::vec3(x, 0.0f, z - 5.0f));
		}
	}

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
	else if (ev.Type == Event::WindowResized)
	{
		m_FB->BindBuffer();
		m_FB->ResizeTexture(ev.Size.Width, ev.Size.Height);
		m_FB->ResizeRenderBuffer(ev.Size.Width, ev.Size.Height);

		Renderer::OnWindowResize(ev.Size.Width, ev.Size.Height);
	}

	m_Camera.OnEvent(ev);
}

void DebugScene::OnInput()
{
}

void DebugScene::OnUpdate(float ts)
{
	m_TS = ts * 1000.0f;

	m_Camera.OnUpdate(ts);
}

void DebugScene::OnRender()
{
	Planet* hovered = nullptr;

	m_FB->BindBuffer();
	m_FB->BindRenderBuffer();

	Renderer::ClearColor(glm::vec4(1.0f));
	Renderer::Clear();

	// 0rd render - color picking
	Renderer::SceneBegin(m_Camera);
	Renderer::EnableDepth();
	Renderer::EndStencil();
	Renderer::SetSphereLightning(false);
	
	for (auto& planet : m_Planets)
	{
		Renderer::SubmitSphereInstanced(planet.GetTransform(), glm::vec4(glm::vec3((float)planet.GetEntityID() / 255.0f), 1.0f));
	}

	Renderer::SceneEnd();

	uint8_t pixelColor = Renderer::GetPixelAt(Input::GetMousePosition()).r;

	for (auto& planet : m_Planets)
	{
		if (planet.GetEntityID() == pixelColor)
		{
			hovered = &planet;

			break;
		}
	}

	Renderer::ClearColor(glm::vec4(0.33f, 0.33f, 0.33f, 1.0f));
	Renderer::Clear();

	// 1st render - draw object with no borders
	Renderer::SceneBegin(m_Camera);
	Renderer::EnableDepth();
	Renderer::EndStencil();
	Renderer::SetSphereLightning(true);

	if (m_ShowGrid)
	{
		DrawGridPlane();
	}

	Renderer::DrawSkybox(m_SkyboxTex);

	for (auto& planet : m_Planets)
	{
		if (&planet == hovered) continue;

		Renderer::SubmitSphereInstanced(planet.GetTransform(), planet.GetColor());
	}

	Renderer::SceneEnd();

	// 2nd render - draw objects with borders, writing to the stencil buffer
	if (hovered)
	{
		Renderer::SceneBegin(m_Camera);
		Renderer::BeginStencil();
		Renderer::SubmitSphereInstanced(hovered->GetTransform(), hovered->GetColor());
		Renderer::SceneEnd();

		Renderer::SceneBegin(m_Camera);
		Renderer::EndStencil();
		Renderer::SetSphereLightning(false);
		Renderer::SubmitSphereInstanced(hovered->GetTransform() * glm::scale(glm::mat4(1.0f), 
			glm::vec3(hovered->GetRadius() * 1.05f)), {0.98f, 0.24f, 0.0f, 1.0f});
		Renderer::SceneEnd();
	}

	Renderer::BeginStencil();

	m_FB->UnbindRenderBuffer();
	m_FB->UnbindBuffer();
	m_FB->BindTexture(1);
}

void DebugScene::OnConfigRender()
{
	glm::vec3 cameraPos = m_Camera.GetPosition();

	ImGui::Begin("Control panel", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	ImGui::NewLine();
	ImGui::Text("Timestep: %.2fms", m_TS);
	ImGui::Text("FPS: %.2f", 1000.0f / m_TS);
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
