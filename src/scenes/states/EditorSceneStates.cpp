#include "EditorSceneStates.hpp"
#include "../EditorScene.hpp"
#include "../../renderer/Renderer.hpp"
#include "../../Input.hpp"
#include "../../Event.hpp"
#include "../../Logger.hpp"

NewSphereState::NewSphereState(EditorScene& scene)
	: m_NewPlanet(Renderer::ScreenToWorldCoords(Input::GetMousePosition(), m_Depth))
	, m_ParentScene(scene)
{
}

void NewSphereState::OnEvent(Event& ev)
{
	if (ev.Type == Event::MouseButtonPressed && ev.MouseButton.Button == MouseButton::Left)
	{
		m_ParentScene.PushNewPlanet(m_NewPlanet);
		m_ParentScene.CancelState();

		return;
	}

	if (ev.Type == Event::KeyPressed || ev.Type == Event::KeyHeld)
	{
		if (ev.Key.Code == Key::KPAdd)
		{
			m_Depth += 0.001f;

			return;
		}

		if (ev.Key.Code == Key::KPSubtract)
		{
			m_Depth -= 0.001f;

			return;
		}
	}
}

void NewSphereState::OnUpdate(float ts)
{
	m_NewPlanet.SetPosition(Renderer::ScreenToWorldCoords(Input::GetMousePosition(), m_Depth));
}

void NewSphereState::OnRender(Camera& camera)
{
	Renderer::SceneBegin(camera);
	Renderer::SetSphereLightning(true);
	Renderer::SubmitSphereInstanced(m_NewPlanet.GetTransform(), m_NewPlanet.GetColor());
	Renderer::SceneEnd();
}

void NewSphereState::OnConfigRender()
{
	m_NewPlanet.OnConfigRender();
}

MoveSphereState::MoveSphereState(EditorScene& scene, Planet* movedPlanet)
	: m_MovedPlanet(movedPlanet), m_ParentScene(scene)
{
	glm::vec3 planetScreenPos = Renderer::WorldToScreenCoords(movedPlanet->GetPosition());
	glm::vec3 mouseScreenPos = glm::vec3(Input::GetMousePosition(), planetScreenPos.z);
	Viewport viewport = Renderer::GetViewport();

	planetScreenPos.x = ((planetScreenPos.x + 1.0f) / 2.0f) * viewport.Width;
	planetScreenPos.y = ((planetScreenPos.y + 1.0f) / 2.0f) * viewport.Height;

	m_Depth = planetScreenPos.z;
	m_Offset = planetScreenPos - mouseScreenPos;
}

void MoveSphereState::OnEvent(Event& ev)
{
	if (ev.Type == Event::MouseButtonReleased && ev.MouseButton.Button == MouseButton::Left)
	{
		m_MovedPlanet = nullptr;
		m_ParentScene.CancelState();

		return;
	}

	if (ev.Type == Event::KeyPressed || ev.Type == Event::KeyHeld)
	{
		if (ev.Key.Code == Key::KPAdd)
		{
			m_Depth += 0.001f;

			return;
		}

		if (ev.Key.Code == Key::KPSubtract)
		{
			m_Depth -= 0.001f;

			return;
		}
	}
}

void MoveSphereState::OnUpdate(float ts)
{
	if (m_MovedPlanet)
	{
		m_MovedPlanet->SetPosition(Renderer::ScreenToWorldCoords(Input::GetMousePosition() + m_Offset, m_Depth));
	}
}

void MoveSphereState::OnRender(Camera& camera)
{
}

void MoveSphereState::OnConfigRender()
{
}
