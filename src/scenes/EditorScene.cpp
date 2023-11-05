#include "EditorScene.hpp"
#include "../Logger.hpp"
#include "../Timer.hpp"
#include "../Event.hpp"
#include "../Application.hpp"
#include "../renderer/Renderer.hpp"
#include "../Random.hpp"
#include "states/EditorSceneStates.hpp"
#include "../Simulator.hpp"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

EditorScene::EditorScene()
{
	FUNC_PROFILE();

	WindowSpec spec = Application::GetInstance()->GetWindowSpec();

	m_FB = std::make_unique<Framebuffer>();
	m_FB->AttachTexture((uint32_t)(spec.Width * 0.8f), spec.Height);
	m_FB->AttachRenderBuffer((uint32_t)(spec.Width * 0.8f), spec.Height);
	m_FB->UnbindBuffer();

	m_MFB = std::make_unique<MultisampledFramebuffer>(16);
	m_MFB->AttachTexture((uint32_t)(spec.Width * 0.8f), spec.Height);
	m_MFB->AttachRenderBuffer((uint32_t)(spec.Width * 0.8f), spec.Height);
	m_MFB->UnbindBuffer();

	Renderer::Init();

	m_Camera.SetPosition(glm::vec3(0.0f, 3.0f, 0.0f));
	m_Camera.MoveYaw(glm::radians(90.0f));
	m_Camera.SetViewportSize({ (float)spec.Width * 0.8f, (float)spec.Height });

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

	m_Planets.emplace_back(glm::vec3(0.0f));
	m_Planets[0].SetMass(1000000.0f);
	m_Planets[0].SetRadius(10.0f);

	m_Planets.emplace_back(glm::vec3(30.0f, 0.0f, 0.0f));
	m_Planets[1].SetMass(10000.0f);

	LOG_INFO("EditorScene initialized.");
}

EditorScene::~EditorScene()
{
	Renderer::Shutdown();

	LOG_INFO("EditorScene destroyed.");
}

void EditorScene::OnEvent(Event& ev)
{
	if (ev.Type == Event::MouseButtonPressed)
	{
		if (ev.MouseButton.Button == MouseButton::Left)
		{
			CheckForPlanetSelect();

			return;
		}
	}

	if (ev.Type == Event::MouseButtonReleased && ev.MouseButton.Button == MouseButton::Right)
	{
		Input::ShowCursor();

		return;
	}

	if (ev.Type == Event::WindowResized)
	{
		m_FB->BindBuffer();
		m_FB->ResizeTexture(ev.Size.Width, ev.Size.Height);
		m_FB->ResizeRenderBuffer(ev.Size.Width, ev.Size.Height);
		m_FB->UnbindBuffer();

		m_MFB->BindBuffer();
		m_MFB->ResizeTexture(ev.Size.Width, ev.Size.Height);
		m_MFB->ResizeRenderBuffer(ev.Size.Width, ev.Size.Height);
		m_MFB->UnbindBuffer();

		Renderer::OnWindowResize({ 0, 0, (int32_t)ev.Size.Width, (int32_t)ev.Size.Height });
		m_Camera.SetViewportSize({ (float)ev.Size.Width, (float)ev.Size.Height });

		return;
	}

	if (ev.Type == Event::KeyPressed)
	{
		switch (ev.Key.Code)
		{
		case Key::Escape:
			m_SelectedPlanet = nullptr;
			m_ActiveState.reset();

			break;

		case Key::F:
			if (m_SelectedPlanet)
			{
				m_ActiveState = std::make_unique<PanderingState>(this, &m_Camera, m_SelectedPlanet);
			}

			break;

		case Key::P:
			Renderer::ToggleWireframe();

			break;

		case Key::LeftAlt:
			if (m_SelectedPlanet && !m_ActiveState)
			{
				m_ActiveState = std::make_unique<SettingVelocityState>(this, &m_Camera, m_SelectedPlanet);
			}

			break;
		}

		return;
	}

	if (m_ActiveState)
	{
		m_ActiveState->OnEvent(ev);

		return;
	}

	m_Camera.OnEvent(ev);
}

void EditorScene::OnInput()
{
	if (!Input::IsMouseButtonPressed(MouseButton::Right) && m_Camera.GetControlType() == CameraControlType::WorldControl)
	{
		Input::ShowCursor();
		m_Camera.SetCameraControlType(CameraControlType::EditorControl);
	}
}

void EditorScene::OnUpdate(float ts)
{
	m_Camera.OnUpdate(ts);

	if (m_ActiveState)
	{
		m_ActiveState->OnUpdate(ts);
	}

	if (!m_Simulate)
	{
		return;
	}

	for (auto& planet : m_Planets)
	{
		planet.OnUpdate(ts);
	}
}

void EditorScene::OnTick()
{
	if (m_ActiveState)
	{
		m_ActiveState->OnTick();
	}

	if (!m_Simulate)
	{
		return;
	}

	SimPhysics::ProgressAllOneStep(m_Planets);
}

void EditorScene::OnRender()
{
	m_MFB->BindBuffer();
	m_MFB->BindRenderBuffer();

	// Draw non selectable stuff
	Renderer::ClearColor(glm::vec4(glm::vec3(0.55f), 1.0f));
	Renderer::Clear();
	
	Renderer::SceneBegin(m_Camera);
	Renderer::EnableDepth();
	Renderer::SetSphereLightning(true);

	if (m_RenderGrid)
	{
		DrawGridPlane();
	}

	if (m_RenderSkybox)
	{
		Renderer::DrawSkybox(m_SkyboxTex);
	}

	Renderer::SceneEnd();

	// Draw spheres' outlines
	Renderer::SceneBegin(m_Camera);
	Renderer::SetSphereLightning(false);
	Renderer::SetFrontCull();

	for (auto& planet : m_Planets)
	{
		if (glm::distance(planet.GetPosition(), m_Camera.GetPosition()) <= planet.GetRadius() * 1.06f)
		{
			continue;
		}

		glm::vec3 color = &planet == m_SelectedPlanet ? glm::vec3(0.98f, 0.24f, 0.0f) : glm::vec3(0.0f);

		Renderer::SubmitSphereInstanced(planet.GetTransform() * glm::scale(glm::mat4(1.0f), glm::vec3(1.05f)), glm::vec4(color, 1.0f));
	}

	Renderer::SceneEnd();
	
	// Draw shaded spheres
	Renderer::SceneBegin(m_Camera);
	Renderer::SetBackCull();
	Renderer::SetSphereLightning(true);

	for (auto& planet : m_Planets)
	{
		Renderer::SubmitSphereInstanced(planet.GetTransform(), planet.GetMaterial());
	}

	Renderer::SceneEnd();

	if (m_ActiveState)
	{
		m_ActiveState->OnRender();
	}

	WindowSpec spec = Application::GetInstance()->GetWindowSpec();

	m_MFB->BlitBuffers((uint32_t)(spec.Width), spec.Height, m_FB->GetFramebufferID());
	m_MFB->UnbindRenderBuffer();
	m_MFB->UnbindBuffer();
}

void EditorScene::SetState(std::unique_ptr<SceneState>&& state)
{
	m_ActiveState = std::move(state);
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
	m_FB->BindBuffer();
	m_FB->BindRenderBuffer();

	PlanetaryObject* hoveredPlanet = nullptr;

	Renderer::ClearColor(glm::vec4(1.0f));
	Renderer::Clear();
	
	Renderer::SceneBegin(m_Camera);
	Renderer::EnableDepth();
	Renderer::SetSphereLightning(false);

	for (auto& planet : m_Planets)
	{
		Renderer::SubmitSphereInstanced(planet.GetTransform(), glm::vec4(glm::vec3((float)planet.GetEntityID() / 255.0f), 1.0f));
	}

	Renderer::SceneEnd();

	WindowSpec spec = Application::GetInstance()->GetWindowSpec();
	uint8_t pixelColor = m_FB->GetPixelAt(Input::GetMousePosition()).r;

	for (auto& planet : m_Planets)
	{
		if (planet.GetEntityID() == pixelColor)
		{
			hoveredPlanet = &planet;

			break;
		}
	}

	if (!m_LockFocusOnPlanet)
	{
		m_SelectedPlanet = hoveredPlanet;
	}
	
	m_FB->UnbindRenderBuffer();
	m_FB->UnbindBuffer();
}

void EditorScene::DrawGridPlane()
{
	constexpr float distance = 400.0f;
	constexpr glm::vec4 lineColor(glm::vec3(0.22f), 1.0f);
	
	glm::vec3 cameraPos = m_Camera.GetPosition();
	int32_t xOffset = cameraPos.x / 4;
	int32_t zOffset = cameraPos.z / 4;

	Renderer::SetLineWidth(1.0f);
	Renderer::LoadLineUniform3f("u_CameraPos", cameraPos);

	for (float x = -distance + xOffset * 4.0f; x <= distance + xOffset * 4.0f; x += 4.0f)
	{
		Renderer::DrawLine({ x, 0.0f, -distance + zOffset * 4.0f }, { x, 0.0f, distance + zOffset * 4.0f },
			x == 0.0f ? glm::vec4(0.98f, 0.24f, 0.0f, 1.0f) : lineColor);
	}

	for (float z = -distance + zOffset * 4.0f; z <= distance + zOffset * 4.0f; z += 4.0f)
	{
		Renderer::DrawLine({ -distance + xOffset * 4.0f, 0.0f, z }, { distance + xOffset * 4.0f, 0.0f, z },
			z == 0.0f ? glm::vec4(0.98f, 0.24f, 0.0f, 1.0f) : lineColor);
	}
}
