#include "EditorSceneStates.hpp"
#include "../EditorScene.hpp"
#include "../../renderer/Renderer.hpp"
#include "../../Input.hpp"
#include "../../Event.hpp"
#include "../../Application.hpp"
#include "../../Logger.hpp"
#include "../Simulator.hpp"

#include <glm/gtx/compatibility.hpp>

InterpolateViewState::InterpolateViewState(EditorScene* scene, Camera* camera, const glm::vec3& targetPos, float targetPitch, float targetYaw)
	: m_TargetPos(targetPos), m_ParentScene(scene), m_EditorCamera(camera)
{
	m_TargetRotation = glm::quat(glm::vec3(-targetPitch, -targetYaw, 0.0f));
}

void InterpolateViewState::OnEvent(Event& ev)
{
}

void InterpolateViewState::OnUpdate(float ts)
{
	glm::vec3 cameraPos = m_EditorCamera->GetPosition();

	if (glm::distance2(m_EditorCamera->GetPosition(), m_TargetPos) < 0.01f * 0.01f)
	{
		m_EditorCamera->SetPosition(m_TargetPos);
		m_ParentScene->CancelState();

		return;
	}

	glm::vec3 dir(0.0f, 0.0f, -glm::distance(cameraPos, glm::vec3(0.0f)));
	glm::quat rot(glm::vec3(-glm::radians(m_EditorCamera->GetPitch()), -glm::radians(m_EditorCamera->GetYaw()), 0.0f));
	glm::vec3 newPos = rot * dir;
	rot = glm::slerp(rot, m_TargetRotation, 10.0f * ts);

	glm::vec3 eulerAngles = glm::eulerAngles(rot);
	m_EditorCamera->SetPitch(-glm::degrees(eulerAngles.x));
	m_EditorCamera->SetYaw(-glm::degrees(eulerAngles.y));
	
	m_EditorCamera->SetPosition(glm::lerp(cameraPos, m_TargetPos, 10.0f * ts));
}

void InterpolateViewState::OnRender()
{
}

SettingVelocityState::SettingVelocityState(EditorScene* scene, Camera* camera, Planet* targetPlanet, const glm::vec2& offset)
	: m_TargetPlanet(targetPlanet), m_Offset(offset), m_EditorCamera(camera), m_ParentScene(scene)
{
}

void SettingVelocityState::OnEvent(Event& ev)
{
	if (ev.Type == Event::KeyReleased && ev.Key.Code == Key::LeftAlt)
	{
		m_ParentScene->CancelState();

		return;
	}

	if (ev.Type == Event::MouseMoved && !m_PathFuture.valid())
	{
		m_PathFuture = std::async(std::launch::async, 
			m_TargetPlanet->GetRelativePlanet() == nullptr ? SimPhysics::ApproximateNextNPoints : SimPhysics::ApproximateRelativeNextNPoints,
			std::ref(m_ParentScene->GetPlanetsRef()), m_TargetPlanet, 1024);
	}
}

void SettingVelocityState::OnUpdate(float ts)
{
	using namespace std::chrono_literals;

	if (m_PathFuture.valid() && m_PathFuture.wait_for(0ms) == std::future_status::ready)
	{
		m_ApproximatedPath = m_PathFuture.get();
	}

	glm::vec3 planetScreenPos = Renderer::WorldToScreenCoords(m_TargetPlanet->GetTransform().Position);
	glm::vec2 mouseScreenPos = Input::GetMousePosition() - m_Offset;
	glm::vec3 mouseWorldPos = Renderer::ScreenToWorldCoords(mouseScreenPos, planetScreenPos.z);

	m_Velocity = m_TargetPlanet->GetTransform().Position - mouseWorldPos;
	m_TargetPlanet->GetPhysics().LinearVelocity = m_Velocity;
}

void SettingVelocityState::OnRender()
{
	if (m_ApproximatedPath.size() < 2)
	{
		return;
	}

	Renderer::SceneBegin(*m_EditorCamera);
	Renderer::SetLineWidth(2.0f);
	Renderer::DisableDepth();
	Renderer::DrawLine(m_TargetPlanet->GetTransform().Position, m_TargetPlanet->GetTransform().Position - m_Velocity, { 1.0f, 0.0f, 0.0f, 1.0f });

	glm::vec3 relativePos = m_TargetPlanet->GetRelativePlanet() ?
		m_TargetPlanet->GetRelativePlanet()->GetTransform().Position : glm::vec3(0.0f, 0.0f, 0.0f);
	for (size_t i = 1; i < m_ApproximatedPath.size(); i++)
	{
		Renderer::DrawLine(m_ApproximatedPath[i - 1] + relativePos, m_ApproximatedPath[i] + relativePos, { 0.0f, 1.0f, 0.0f, 1.0f });
	}

	Renderer::SceneEnd();
}

PanderingState::PanderingState(EditorScene* scene, Camera* camera, Planet* targetPlanet)
	: m_ParentScene(scene), m_EditorCamera(camera)
{
	glm::vec3 normalizedCameraForward = glm::normalize(camera->GetForwardDirection());

	m_TargetPos = targetPlanet->GetTransform().Position - normalizedCameraForward * 6.0f * targetPlanet->GetMaxRadius();
	m_DeltaMove = m_TargetPos - camera->GetPosition();
}

void PanderingState::OnEvent(Event& ev)
{
}

void PanderingState::OnUpdate(float ts)
{
	glm::vec3 cameraPos = m_EditorCamera->GetPosition();
	bool skippedTarget = glm::dot(m_DeltaMove, m_TargetPos - cameraPos) < 0.0f;

	if (skippedTarget || glm::distance2(cameraPos, m_TargetPos) < 0.01f * 0.01f)
	{
		m_EditorCamera->SetPosition(m_TargetPos);
		m_ParentScene->CancelState();

		return;
	}

	m_EditorCamera->SetPosition(glm::lerp(cameraPos, m_TargetPos, 10.0f * ts));
}

void PanderingState::OnRender()
{
}

FollowingPlanetState::FollowingPlanetState(EditorScene* scene, Camera* camera, Planet* targetPlanet)
	: m_ParentScene(scene), m_EditorCamera(camera), m_TargetPlanet(targetPlanet)
{
	glm::vec3 normalizedCameraForward = glm::normalize(camera->GetForwardDirection());
	glm::vec3 planetPos = targetPlanet->GetTransform().Position;

	m_DistanceToCamera = m_TargetDistanceToCamera = glm::distance(planetPos, m_EditorCamera->GetPosition());
	m_EditorCamera->SetPosition(m_TargetPlanet->GetTransform().Position - normalizedCameraForward * m_DistanceToCamera);
}

void FollowingPlanetState::OnEvent(Event& ev)
{
	if (ev.Type == Event::MouseWheelScrolled)
	{
		m_TargetDistanceToCamera -= ev.MouseWheel.OffsetY * 2.0f;

		return;
	}
}

void FollowingPlanetState::OnUpdate(float ts)
{
	if (m_ParentScene->SelectedPlanet() != m_TargetPlanet)
	{
		m_ParentScene->CancelState();

		return;
	}

	glm::vec3 normalizedCameraForward = glm::normalize(m_EditorCamera->GetForwardDirection());
	glm::vec3 planetPos = m_TargetPlanet->GetTransform().Position;

	m_DistanceToCamera = std::lerp(m_DistanceToCamera, m_TargetDistanceToCamera, 10.0f * ts);
	m_EditorCamera->SetPosition(m_TargetPlanet->GetTransform().Position - normalizedCameraForward * m_DistanceToCamera);
}

void FollowingPlanetState::OnRender()
{
}
