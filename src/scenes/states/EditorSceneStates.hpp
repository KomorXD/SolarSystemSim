#pragma once

#include "SceneState.hpp"
#include "../../objects/Planet.hpp"

class EditorScene;

class NewSphereState : public SceneState
{
public:
	NewSphereState(EditorScene& scene);
	~NewSphereState();

	virtual void OnEvent(Event& ev) override;
	virtual void OnUpdate(float ts) override;
	virtual void OnRender()			override;

private:
	Planet m_NewPlanet;
	float m_Depth = 0.9f;

	EditorScene& m_ParentScene;
};