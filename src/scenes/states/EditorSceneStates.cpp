#include "EditorSceneStates.hpp"
#include "../EditorScene.hpp"
#include "../../renderer/Renderer.hpp"
#include "../../Input.hpp"
#include "../../Event.hpp"
#include "../../Logger.hpp"

NewPlanetState::NewPlanetState(EditorScene& scene, Planet& newPlanet)
	: m_NewPlanet(newPlanet)
	, m_ParentScene(scene)
{
}

void NewPlanetState::OnEvent(Event& ev)
{
	if (ev.Type == Event::MouseButtonPressed && ev.MouseButton.Button == MouseButton::Left)
	{
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

void NewPlanetState::OnUpdate(float ts)
{
	m_NewPlanet.SetPosition(Renderer::ScreenToWorldCoords(Input::GetMousePosition(), m_Depth));
}

void NewPlanetState::OnRender(Camera& camera)
{
	Renderer::SceneBegin(camera);
	Renderer::SetSphereLightning(true);
	Renderer::SubmitSphereInstanced(m_NewPlanet.GetTransform(), m_NewPlanet.GetColor());
	Renderer::SceneEnd();
}

void NewPlanetState::OnConfigRender()
{
	m_NewPlanet.OnConfigRender();
}

MovePlanetState::MovePlanetState(EditorScene& scene, Planet& movedPlanet)
	: m_MovedPlanet(movedPlanet), m_ParentScene(scene)
{
	glm::vec3 planetScreenPos = Renderer::WorldToScreenCoords(movedPlanet.GetPosition());
	glm::vec3 mouseScreenPos = glm::vec3(Input::GetMousePosition(), planetScreenPos.z);
	Viewport viewport = Renderer::GetViewport();

	planetScreenPos.x = ((planetScreenPos.x + 1.0f) / 2.0f) * viewport.Width;
	planetScreenPos.y = ((planetScreenPos.y + 1.0f) / 2.0f) * viewport.Height;

	m_Depth = planetScreenPos.z;
	m_Offset = planetScreenPos - mouseScreenPos;
}

void MovePlanetState::OnEvent(Event& ev)
{
	if (ev.Type == Event::MouseButtonReleased && ev.MouseButton.Button == MouseButton::Left)
	{
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

void MovePlanetState::OnUpdate(float ts)
{
	m_MovedPlanet.SetPosition(Renderer::ScreenToWorldCoords(Input::GetMousePosition() + m_Offset, m_Depth));
}

void MovePlanetState::OnRender(Camera& camera)
{
}

void MovePlanetState::OnConfigRender()
{
}

InterpolateViewState::InterpolateViewState(EditorScene& scene, Camera& camera, const glm::vec3& targetPos, float targetPitch, float targetYaw)
	: m_TargetPos(targetPos), m_TargetPitch(targetPitch), m_TargetYaw(targetYaw), m_ParentScene(scene), m_SceneCamera(camera)
{
	m_DeltaMove  = targetPos - camera.GetPosition();
	m_DeltaPitch = targetPitch - camera.GetPitch();
	m_DeltaYaw   = targetYaw - camera.GetYaw();
}

void InterpolateViewState::OnEvent(Event& ev)
{
}

void InterpolateViewState::OnUpdate(float ts)
{
	if (glm::distance(m_SceneCamera.GetPosition(), m_TargetPos) < 0.001f)
	{
		m_ParentScene.CancelState();

		return;
	}

	m_SceneCamera.Move(m_DeltaMove * 0.05f);
	m_SceneCamera.MovePitch(m_DeltaPitch * 0.05f);
	m_SceneCamera.MoveYaw(m_DeltaYaw * 0.05f);
}

void InterpolateViewState::OnRender(Camera& camera)
{
}

void InterpolateViewState::OnConfigRender()
{
}
