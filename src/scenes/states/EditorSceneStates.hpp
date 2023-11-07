#pragma once

#include "SceneState.hpp"
#include "../../objects/Planet.hpp"

#include <vector>
#include <future>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

class EditorScene;

class NewPlanetState : public SceneState
{
public:
	NewPlanetState(EditorScene* scene, PlanetaryObject* newPlanet);

	virtual void OnEvent(Event& ev) override;
	virtual void OnUpdate(float ts) override;
	virtual void OnTick()			override;
	virtual void OnRender()			override;

private:
	PlanetaryObject* m_NewPlanet = nullptr;
	float m_Depth = 0.97f;

	EditorScene* m_ParentScene = nullptr;
};

class InterpolateViewState : public SceneState
{
public:
	InterpolateViewState(EditorScene* scene, Camera* camera, const glm::vec3& targetPos, float targetPitch, float targetYaw);

	virtual void OnEvent(Event& ev) override;
	virtual void OnUpdate(float ts) override;
	virtual void OnTick()			override;
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
	SettingVelocityState(EditorScene* scene, Camera* camera, PlanetaryObject* targetPlanet);

	virtual void OnEvent(Event& ev) override;
	virtual void OnUpdate(float ts) override;
	virtual void OnTick()			override;
	virtual void OnRender()			override;

private:
	glm::vec3 m_Velocity = { 0.0f, 0.0f, 0.0f };
	std::vector<glm::vec3> m_ApproximatedPath;
	std::future<std::vector<glm::vec3>> m_PathFuture;

	PlanetaryObject* m_TargetPlanet = nullptr;

	Camera* m_EditorCamera = nullptr;
	EditorScene* m_ParentScene = nullptr;
};

class PanderingState : public SceneState
{
public:
	PanderingState(EditorScene* scene, Camera* camera, PlanetaryObject* targetPlanet);

	virtual void OnEvent(Event& ev) override;
	virtual void OnUpdate(float ts) override;
	virtual void OnTick()			override;
	virtual void OnRender()			override;

private:
	glm::vec3 m_TargetPos = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_DeltaMove = { 0.0f, 0.0f, 0.0f };;

	Camera* m_EditorCamera = nullptr;
	EditorScene* m_ParentScene = nullptr;
};