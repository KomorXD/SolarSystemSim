#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "SceneState.hpp"

#include <vector>
#include <future>

class EditorScene;
class Planet;

class InterpolateViewState : public SceneState
{
public:
	InterpolateViewState(EditorScene* scene, Camera* camera, const glm::vec3& targetPos, float targetPitch, float targetYaw);

	virtual void OnEvent(Event& ev) override;
	virtual void OnUpdate(float ts) override;
	virtual void OnRender()			override;

private:
	glm::quat m_TargetRotation{};
	glm::vec3 m_TargetPos = { 0.0f, 0.0f, 0.0f };;
	
	Camera* m_EditorCamera = nullptr;
	EditorScene* m_ParentScene = nullptr;
};

class SettingVelocityState : public SceneState
{
public:
	SettingVelocityState(EditorScene* scene, Camera* camera, Planet* targetPlanet, const glm::vec2& offset);

	virtual void OnEvent(Event& ev) override;
	virtual void OnUpdate(float ts) override;
	virtual void OnRender()			override;

private:
	glm::vec3 m_Velocity = { 0.0f, 0.0f, 0.0f };
	glm::vec2 m_Offset   = { 0.0f, 0.0f };
	std::vector<glm::vec3> m_ApproximatedPath;
	std::future<std::vector<glm::vec3>> m_PathFuture;

	Planet* m_TargetPlanet = nullptr;

	Camera* m_EditorCamera = nullptr;
	EditorScene* m_ParentScene = nullptr;
};

class PanderingState : public SceneState
{
public:
	PanderingState(EditorScene* scene, Camera* camera, Planet* targetPlanet);

	virtual void OnEvent(Event& ev) override;
	virtual void OnUpdate(float ts) override;
	virtual void OnRender()			override;

private:
	glm::vec3 m_TargetPos = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_DeltaMove = { 0.0f, 0.0f, 0.0f };

	Camera* m_EditorCamera = nullptr;
	EditorScene* m_ParentScene = nullptr;
};

class FollowingPlanetState : public SceneState
{
public:
	FollowingPlanetState(EditorScene* scene, Camera* camera, Planet* targetPlanet);

	virtual void OnEvent(Event& ev) override;
	virtual void OnUpdate(float ts) override;
	virtual void OnRender()			override;

private:
	Camera* m_EditorCamera = nullptr;
	EditorScene* m_ParentScene = nullptr;
	Planet* m_TargetPlanet = nullptr;

	float m_TargetDistanceToCamera{};
	float m_DistanceToCamera{};
};
