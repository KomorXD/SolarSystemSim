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
	m_FB->AttachTexture(spec.Width * 0.66f, spec.Height);
	m_FB->AttachRenderBuffer(spec.Width * 0.66f, spec.Height);
	m_FB->UnbindBuffer();

	Renderer::Init();
	Renderer::OnWindowResize(spec.Width * 0.66f, spec.Height);

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

struct Sphere
{
	glm::vec3 Origin;
	glm::vec4 Color;
	float Radius = 1.0f;

	uint32_t EntityID = 0;
};

uint32_t s_IdTracker = 1;

Sphere CreateSphere(const glm::vec3& center, const glm::vec4& color, float radius)
{
	Sphere sphere{};

	sphere.Origin = center;
	sphere.Color = color;
	sphere.Radius = radius;
	sphere.EntityID = s_IdTracker;

	++s_IdTracker;

	return sphere;
}

void DebugScene::OnRender()
{
	s_IdTracker = 1;

	Sphere s1 = CreateSphere({  2.0f, 0.0f, -10.0f }, m_SphereColor, 0.5f);
	Sphere s2 = CreateSphere({ -2.0f, 0.0f, -10.0f }, m_SphereColor, 1.0f);
	Sphere* hovered = nullptr;

	m_FB->BindBuffer();
	m_FB->BindRenderBuffer();

	Renderer::ClearColor(glm::vec4(1.0f));
	Renderer::Clear();

	// 0rd render - color picking
	Renderer::SceneBegin(m_Camera);
	Renderer::EnableDepth();
	Renderer::EndStencil();
	Renderer::SetSphereLightning(false);
	Renderer::DrawSphere(s1.Origin, s1.Radius, glm::vec4(glm::vec3((float)s1.EntityID / (s_IdTracker)), 1.0f));
	Renderer::DrawSphere(s2.Origin, s2.Radius, glm::vec4(glm::vec3((float)s2.EntityID / (s_IdTracker)), 1.0f));
	Renderer::SceneEnd();

	float pixelColor = Renderer::GetPixelAt(Input::GetMousePosition()).r;

	if (pixelColor == (float)s1.EntityID / s_IdTracker)
	{
		hovered = &s1;
	}
	else if(pixelColor == (float)s2.EntityID / s_IdTracker)
	{
		hovered = &s2;
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
	Renderer::SceneEnd();

	// 2nd render - draw objects with borders, writing to the stencil buffer
	Renderer::SceneBegin(m_Camera);
	Renderer::BeginStencil();
	Renderer::DrawSphere(s1.Origin, s1.Radius, s1.Color);
	Renderer::DrawSphere(s2.Origin, s2.Radius, s2.Color);
	Renderer::SceneEnd();

	// 3rd render - draw objects with borders using stencil buffer, outlining them
	if (hovered)
	{
		Renderer::SceneBegin(m_Camera);
		Renderer::EndStencil();
		Renderer::SetSphereLightning(false);

		Renderer::DrawSphere(hovered->Origin, hovered->Radius * 1.05f, { 0.98f, 0.24f, 0.0f, 1.0f });

		Renderer::SceneEnd();
		Renderer::BeginStencil();
	}	

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
