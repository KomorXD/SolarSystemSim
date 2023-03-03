#pragma once

#include "SceneState.hpp"
#include "../../objects/Planet.hpp"

class EditorScene;

class NewPlanetState : public SceneState
{
public:
	NewPlanetState(EditorScene* scene, Planet* newPlanet);

	virtual void OnEvent(Event& ev) override;
	virtual void OnUpdate(float ts) override;
	virtual void OnRender()			override;

private:
	Planet* m_NewPlanet = nullptr;
	float m_Depth = 0.97f;

	EditorScene* m_ParentScene = nullptr;
};

class InterpolateViewState : public SceneState
{
public:
	InterpolateViewState(EditorScene* scene, Camera* camera, const glm::vec3& targetPos, float targetPitch, float targetYaw);

	virtual void OnEvent(Event& ev) override;
	virtual void OnUpdate(float ts) override;
	virtual void OnRender()			override;

private:
	glm::vec3 m_TargetPos = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_DeltaMove = { 0.0f, 0.0f, 0.0f };

	float m_TargetPitch = 0.0f;
	float m_DeltaPitch  = 0.0f;

	float m_TargetYaw = 0.0f;
	float m_DeltaYaw  = 0.0f;

	Camera* m_EditorCamera = nullptr;
	EditorScene* m_ParentScene = nullptr;
};

class SettingVelocityState : public SceneState
{
public:
	SettingVelocityState(EditorScene* scene, Camera* camera, Planet* targetPlanet);

	virtual void OnEvent(Event& ev) override;
	virtual void OnUpdate(float ts) override;
	virtual void OnRender()			override;

private:
	glm::vec3 m_Velocity = { 0.0f, 0.0f, 0.0f };

	Planet* m_TargetPlanet = nullptr;

	Camera* m_EditorCamera = nullptr;
	EditorScene* m_ParentScene = nullptr;
};