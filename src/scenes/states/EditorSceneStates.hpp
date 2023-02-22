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
	Planet* m_MovedPlanet;
	glm::vec2 m_Offset;
	float m_Depth = 1.0f;

	EditorScene& m_ParentScene;
};