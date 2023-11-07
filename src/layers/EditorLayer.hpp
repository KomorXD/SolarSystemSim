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

private:
	void RenderViewport();
	void RenderImGuizmo();
	void RenderControlPanel();
	void RenderPlanetsCombo();
	void RenderEntityData();

	std::unique_ptr<EditorScene> m_Scene;

	int32_t m_GizmoMode = -1;

	bool m_IsViewportFocused = true;
};