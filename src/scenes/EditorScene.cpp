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

	m_Camera.SetPosition(glm::vec3(0.0f, 3.0f, 0.0f));
	m_Camera.MoveYaw(glm::radians(90.0f));
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

	float xd = 5.0f;

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

		Renderer::OnWindowResize({ 0, 0, (int32_t)ev.Size.Width, (int32_t)ev.Size.Height });
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

	Renderer::ClearColor(glm::vec4(glm::vec3(0.55f), 1.0f));
	Renderer::Clear();

	// 1st render - draw object with no borders
	Renderer::SceneBegin(m_Camera);
	Renderer::EnableDepth();
	Renderer::EndStencil();
	Renderer::SetSphereLightning(true);

	if (m_RenderGrid)
	{
		DrawGridPlane();
	}

	if (m_RenderSkybox)
	{
		Renderer::DrawSkybox(m_SkyboxTex);
	}

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
			glm::vec3(m_SelectedPlanet->GetRadius() + 0.05f)), { 0.98f, 0.24f, 0.0f, 1.0f });
		Renderer::SceneEnd();
	}

	Renderer::BeginStencil();

	m_FB->UnbindRenderBuffer();
	m_FB->UnbindBuffer();
	m_FB->BindTexture(1);
}

void EditorScene::SetState(std::unique_ptr<SceneState>&& state)
{
	m_ActiveState = std::move(state);
}

SceneData EditorScene::GetSceneData()
{
	SceneData data{};

	data.Planets = &m_Planets;
	data.SelectedPlanet = &m_SelectedPlanet;
	data.EditorCamera = &m_Camera;
	data.ShowGrid = &m_RenderGrid;

	return data;
}

uint32_t EditorScene::GetFramebufferTextureID() const
{
	return m_FB->GetTextureID();
}

void EditorScene::CancelState()
{
	m_ActiveState.reset();
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

	WindowSpec spec = Application::GetInstance()->GetWindowSpec();
	uint8_t pixelColor = Renderer::GetPixelAt(Input::GetMousePosition()).r;

	for (auto& planet : m_Planets)
	{
		if (planet.GetEntityID() == pixelColor)
		{
			hoveredPlanet = &planet;

			break;
		}
	}

	if (Input::IsMouseButtonPressed(MouseButton::Left) && hoveredPlanet)
	{
		m_SelectedPlanet = hoveredPlanet;
	}
}

void EditorScene::DrawGridPlane()
{
	constexpr float distance = 30.0f;
	constexpr glm::vec4 lineColor(glm::vec3(0.22f), 1.0f);

	Renderer::SetLineWidth(1.0f);

	for (float x = -distance; x <= distance; x += 2.0f)
	{
		Renderer::DrawLine({ x, 0.0f, -distance }, { x, 0.0f, distance },
			x == 0.0f ? glm::vec4(0.98f, 0.24f, 0.0f, 1.0f) : lineColor);
	}

	for (float z = -distance; z <= distance; z += 2.0f)
	{
		Renderer::DrawLine({ -distance, 0.0f, z }, { distance, 0.0f, z }, 
			z == 0.0f ? glm::vec4(0.98f, 0.24f, 0.0f, 1.0f) : lineColor);
	}
}
