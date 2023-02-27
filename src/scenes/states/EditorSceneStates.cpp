#include "EditorSceneStates.hpp"
#include "../EditorScene.hpp"
#include "../../renderer/Renderer.hpp"
#include "../../Input.hpp"
#include "../../Event.hpp"
#include "../../Application.hpp"
#include "../../Logger.hpp"

NewPlanetState::NewPlanetState(EditorScene* scene, Planet* newPlanet)
	: m_NewPlanet(newPlanet)
	, m_ParentScene(scene)
{
}

void NewPlanetState::OnEvent(Event& ev)
{
	if (ev.Type == Event::MouseButtonPressed && ev.MouseButton.Button == MouseButton::Left)
	{
		m_ParentScene->CancelState();

		return;
	}

	if (ev.Type == Event::MouseWheelScrolled)
	{
		m_Depth += 0.001f * ev.MouseWheel.OffsetY;

		return;
	}
}

void NewPlanetState::OnUpdate(float ts)
{
	WindowSpec spec = Application::GetInstance()->GetWindowSpec();

	m_NewPlanet->SetPosition(Renderer::ScreenToWorldCoords(Input::GetMousePosition(), m_Depth));
}

InterpolateViewState::InterpolateViewState(EditorScene* scene, Camera* camera, const glm::vec3& targetPos, float targetPitch, float targetYaw)
	: m_TargetPos(targetPos), m_TargetPitch(targetPitch), m_TargetYaw(targetYaw), m_ParentScene(scene), m_SceneCamera(camera)
{
	m_DeltaMove  = targetPos - camera->GetPosition();
	m_DeltaPitch = targetPitch - camera->GetPitch();
	m_DeltaYaw   = targetYaw - camera->GetYaw();
}

void InterpolateViewState::OnEvent(Event& ev)
{
}

void InterpolateViewState::OnUpdate(float ts)
{
	if (glm::distance(m_SceneCamera->GetPosition(), m_TargetPos) < 0.001f)
	{
		m_ParentScene->CancelState();

		return;
	}

	m_SceneCamera->Move(m_DeltaMove * 0.05f);
	m_SceneCamera->MovePitch(m_DeltaPitch * 0.05f);
	m_SceneCamera->MoveYaw(m_DeltaYaw * 0.05f);
}
