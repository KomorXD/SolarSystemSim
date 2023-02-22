#include "EditorScene.hpp"
#include "../Logger.hpp"
#include "../Timer.hpp"
#include "../Event.hpp"
#include "../Application.hpp"
#include "../renderer/Renderer.hpp"
#include "../Random.hpp"
#include "states/EditorSceneStates.hpp"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

EditorScene::EditorScene()
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
			m_Planets.back().SetColor(
				{
					Random::FloatInRange(0.0f, 1.0f),
					Random::FloatInRange(0.0f, 1.0f),
					Random::FloatInRange(0.0f, 1.0f),
					1.0f
				});
		}
	}

	LOG_INFO("EditorScene initialized.");
}

EditorScene::~EditorScene()
{
	Renderer::Shutdown();

	LOG_INFO("EditorScene destroyed.");
}

void EditorScene::OnEvent(Event& ev)
{
	if (ev.Type == Event::WindowResized)
	{
		m_FB->BindBuffer();
		m_FB->ResizeTexture(ev.Size.Width, ev.Size.Height);
		m_FB->ResizeRenderBuffer(ev.Size.Width, ev.Size.Height);

		Renderer::OnWindowResize(ev.Size.Width, ev.Size.Height);
		m_Camera.SetViewportSize({ (float)ev.Size.Width, (float)ev.Size.Height });

		return;
	}

	if (ev.Type == Event::KeyPressed && ev.Key.Code == Key::Escape)
	{
		m_SelectedPlanet = nullptr;
		m_ActiveState.reset();

		return;
	}

	if (m_ActiveState)
	{
		m_ActiveState->OnEvent(ev);

		return;
	}

	if (ev.Type == Event::MouseButtonPressed && ev.MouseButton.Button == MouseButton::Left)
	{
		CheckForPlanetSelect();

		return;
	}

	m_Camera.OnEvent(ev);
}

void EditorScene::OnInput()
{
}

void EditorScene::OnUpdate(float ts)
{
	m_TS = ts * 1000.0f;
	m_Camera.OnUpdate(ts);

	if (m_ActiveState)
	{
		m_ActiveState->OnUpdate(ts);
	}
}

void EditorScene::OnRender()
{
	m_FB->BindBuffer();
	m_FB->BindRenderBuffer();

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
		if (&planet == m_SelectedPlanet) continue;

		Renderer::SubmitSphereInstanced(planet.GetTransform(), planet.GetColor());
	}

	Renderer::SceneEnd();

	// 2nd render - draw objects with borders, writing to the stencil buffer
	if (m_SelectedPlanet)
	{
		Renderer::SceneBegin(m_Camera);
		Renderer::BeginStencil();
		Renderer::SubmitSphereInstanced(m_SelectedPlanet->GetTransform(), m_SelectedPlanet->GetColor());
		Renderer::SceneEnd();

		Renderer::SceneBegin(m_Camera);
		Renderer::EndStencil();
		Renderer::SetSphereLightning(false);
		Renderer::SubmitSphereInstanced(m_SelectedPlanet->GetTransform() * glm::scale(glm::mat4(1.0f),
			glm::vec3(m_SelectedPlanet->GetRadius() + 0.05f)), {0.98f, 0.24f, 0.0f, 1.0f});
		Renderer::SceneEnd();
	}

	if (m_ActiveState)
	{
		m_ActiveState->OnRender(m_Camera);
	}

	Renderer::BeginStencil();

	m_FB->UnbindRenderBuffer();
	m_FB->UnbindBuffer();
	m_FB->BindTexture(1);
}

void EditorScene::OnConfigRender()
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

	ImGui::Checkbox("Show grid", &m_ShowGrid);
	
	if (ImGui::Button("New planet"))
	{
		m_SelectedPlanet = nullptr;
		m_ActiveState = std::make_unique<NewSphereState>(*this);
	}

	ImGui::SameLine();

	if (ImGui::Button("Toggle wireframe"))
	{
		Renderer::ToggleWireframe();
	}

	ImGui::NewLine();
	ImGui::Separator();
	ImGui::NewLine();

	if (m_ActiveState)
	{
		m_ActiveState->OnConfigRender();
	}

	if (m_SelectedPlanet)
	{
		m_SelectedPlanet->OnConfigRender();
	}

	ImGui::End();
}

uint32_t EditorScene::GetFramebufferTextureID() const
{
	return m_FB->GetTextureID();
}

void EditorScene::CancelState()
{
	m_ActiveState.reset();
}

void EditorScene::PushNewPlanet(Planet& planet)
{
	m_Planets.push_back(planet);
}

void EditorScene::CheckForPlanetSelect()
{
	Planet* hoveredPlanet = nullptr;

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
			hoveredPlanet = &planet;

			break;
		}
	}

	if (Input::IsMouseButtonPressed(MouseButton::Left))
	{
		m_SelectedPlanet = hoveredPlanet ? hoveredPlanet : nullptr;
	}

	if (m_SelectedPlanet)
	{
		m_ActiveState.reset();
		m_ActiveState = std::make_unique<MoveSphereState>(*this, m_SelectedPlanet);
	}
}

void EditorScene::DrawGridPlane()
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
