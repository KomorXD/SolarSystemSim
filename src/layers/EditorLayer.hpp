#pragma once

#include "Layer.hpp"
#include "../scenes/Scene.hpp"

#include <memory>

class EditorLayer : public Layer
{
public:
	EditorLayer();

	virtual void OnEvent(Event& ev) override;
	virtual void OnInput()			override;
	virtual void OnUpdate(float ts) override;
	virtual void OnImGuiRender()	override;

private:
	void RenderViewport();
	void RenderImGuizmo();
	void RenderControlPanel();
	void RenderEntityData();

	std::unique_ptr<Scene> m_Scene;
	SceneData m_SceneData;

	int32_t m_GizmoMode = -1;

	bool m_IsViewportFocused = true;
};