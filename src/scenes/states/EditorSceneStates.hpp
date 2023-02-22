#pragma once

#include "SceneState.hpp"
#include "../../objects/Planet.hpp"

class EditorScene;

class NewSphereState : public SceneState
{
public:
	NewSphereState(EditorScene& scene);

	virtual void OnEvent(Event& ev)		  override;
	virtual void OnUpdate(float ts)		  override;
	virtual void OnRender(Camera& camera) override;
	virtual void OnConfigRender()		  override;

private:
	Planet m_NewPlanet;
	float m_Depth = 0.98f;

	EditorScene& m_ParentScene;
};

class MoveSphereState : public SceneState
{
public:
	MoveSphereState(EditorScene& scene, Planet* movedPlanet);

	virtual void OnEvent(Event& ev)		  override;
	virtual void OnUpdate(float ts)		  override;
	virtual void OnRender(Camera& camera) override;
	virtual void OnConfigRender()		  override;

private:
	Planet* m_MovedPlanet = nullptr;
	glm::vec2 m_Offset;
	float m_Depth = 1.0f;

	EditorScene& m_ParentScene;
};

class InterpolateViewState : public SceneState
{
public:
	InterpolateViewState(EditorScene& scene, Camera& camera, const glm::vec3& targetPos, float targetPitch, float targetYaw);

	virtual void OnEvent(Event& ev)		  override;
	virtual void OnUpdate(float ts)		  override;
	virtual void OnRender(Camera& camera) override;
	virtual void OnConfigRender()		  override;

private:
	glm::vec3 m_TargetPos = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_DeltaMove = { 0.0f, 0.0f, 0.0f };

	float m_TargetPitch = 0.0f;
	float m_DeltaPitch  = 0.0f;

	float m_TargetYaw  = 0.0f;
	float m_DeltaYaw   = 0.0f;

	EditorScene& m_ParentScene;
	Camera& m_SceneCamera;
};