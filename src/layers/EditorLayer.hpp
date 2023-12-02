#pragma once

#include "Layer.hpp"
#include "../scenes/EditorScene.hpp"

#include <memory>

class EditorLayer : public Layer
{
public:
	EditorLayer();

	virtual void OnEvent(Event& ev) override;
	virtual void OnInput()			override;
	virtual void OnUpdate(float ts) override;
	virtual void OnTick()			override;
	virtual void OnImGuiRender()	override;
	virtual void OnAttach()			override;

private:
	void RenderScenePanel();
	void RenderViewport();
	void RenderImGuizmo();
	void RenderTopbar();
	void RenderControlPanel();
	void RenderPlanetsCombo();
	void RenderEntityData();

	std::unique_ptr<EditorScene> m_Scene;

	int32_t m_GizmoMode = -1; // None
	int32_t m_GizmoCoords = 1; // World

	float m_TopbarHeight = 40.0f;

	bool m_IsViewportFocused = true;
};