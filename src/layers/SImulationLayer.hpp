#pragma once

#include "Layer.hpp"
#include "../scenes/EditorScene.hpp"

class SimulationLayer : public Layer
{
public:
	SimulationLayer(std::unique_ptr<EditorScene>& scene);

	virtual void OnEvent(Event& ev) override;
	virtual void OnInput()			override;
	virtual void OnUpdate(float ts) override;
	virtual void OnImGuiRender()	override;

private:
	std::unique_ptr<Scene> m_ActiveScene;
};