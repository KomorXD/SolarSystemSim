#pragma once

#include "Layer.hpp"
#include "../scenes/Scene.hpp"

class SimulationLayer : public Layer
{
public:
	SimulationLayer();

	virtual void OnEvent(Event& ev) override;
	virtual void OnInput()			override;
	virtual void OnUpdate(float ts) override;
	virtual void OnImGuiRender()	override;

private:
	std::unique_ptr<Scene> m_ActiveScene;
};