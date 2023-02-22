#include "EditorSceneStates.hpp"
#include "../EditorScene.hpp"
#include "../../renderer/Renderer.hpp"
#include "../../Input.hpp"
#include "../../Event.hpp"

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
