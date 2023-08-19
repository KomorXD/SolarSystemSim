#include "EditorSceneStates.hpp"
#include "../EditorScene.hpp"
#include "../../renderer/Renderer.hpp"
#include "../../Input.hpp"
#include "../../Event.hpp"
#include "../../Application.hpp"
#include "../../Logger.hpp"

NewPlanetState::NewPlanetState(EditorScene* scene, PlanetaryObject* newPlanet)
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

void NewPlanetState::OnRender()
{
}

InterpolateViewState::InterpolateViewState(EditorScene* scene, Camera* camera, const glm::vec3& targetPos, float targetPitch, float targetYaw)
	: m_TargetPos(targetPos), m_TargetPitch(targetPitch), m_TargetYaw(targetYaw), m_ParentScene(scene), m_EditorCamera(camera)
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
	if (glm::distance(m_EditorCamera->GetPosition(), m_TargetPos) < 0.001f)
	{
		m_ParentScene->CancelState();

		return;
	}

	m_EditorCamera->Move(m_DeltaMove * 0.05f);
	m_EditorCamera->MovePitch(m_DeltaPitch * 0.05f);
	m_EditorCamera->MoveYaw(m_DeltaYaw * 0.05f);
}

void InterpolateViewState::OnRender()
{
}

SettingVelocityState::SettingVelocityState(EditorScene* scene, Camera* camera, PlanetaryObject* targetPlanet)
	: m_TargetPlanet(targetPlanet), m_EditorCamera(camera), m_ParentScene(scene)
{
}

void SettingVelocityState::OnEvent(Event& ev)
{
	if (ev.Type == Event::MouseButtonReleased && ev.MouseButton.Button == MouseButton::Right)
	{
		m_ParentScene->CancelState();

		return;
	}
}

void SettingVelocityState::OnUpdate(float ts)
{
	glm::vec3 planetScreenPos = Renderer::WorldToScreenCoords(m_TargetPlanet->GetPosition());
	glm::vec2 mouseScreenPos = Input::GetMousePosition();
	glm::vec3 mouseWorldPos = Renderer::ScreenToWorldCoords(mouseScreenPos, planetScreenPos.z);

	m_Velocity = m_TargetPlanet->GetPosition() - mouseWorldPos;
	m_TargetPlanet->SetVelocity(m_Velocity);
}

void SettingVelocityState::OnRender()
{
	Renderer::SceneBegin(*m_EditorCamera);
	Renderer::SetLineWidth(5.0f);
	Renderer::DisableDepth();
	Renderer::DrawLine(m_TargetPlanet->GetPosition(), m_TargetPlanet->GetPosition() - m_Velocity, { 1.0f, 0.0f, 0.0f, 1.0f });
	Renderer::SceneEnd();
}

PanderingState::PanderingState(EditorScene* scene, Camera* camera, PlanetaryObject* targetPlanet)
	: m_ParentScene(scene), m_EditorCamera(camera)
{
	glm::vec3 normalizedCameraForward = glm::normalize(camera->GetForwardDirection());

	m_TargetPos = targetPlanet->GetPosition() - normalizedCameraForward * 6.0f * targetPlanet->GetRadius();
	m_DeltaMove = m_TargetPos - camera->GetPosition();
}

void PanderingState::OnEvent(Event& ev)
{
}

void PanderingState::OnUpdate(float ts)
{
	if (glm::distance(m_EditorCamera->GetPosition(), m_TargetPos) < 0.001f)
	{
		m_ParentScene->CancelState();

		return;
	}

	m_EditorCamera->Move(m_DeltaMove * 0.05f);
}

void PanderingState::OnRender()
{
}
