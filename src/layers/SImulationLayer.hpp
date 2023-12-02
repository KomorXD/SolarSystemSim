#pragma once

#include "Layer.hpp"

#include <memory>

class EditorScene;

class SimulationLayer : public Layer
{
public:
	SimulationLayer(std::unique_ptr<EditorScene>& scene);
	virtual ~SimulationLayer();

	virtual void OnEvent(Event& ev) override;
	virtual void OnInput()			override;
	virtual void OnUpdate(float ts) override;
	virtual void OnTick()			override;
	virtual void OnImGuiRender()	override;
	virtual void OnAttach()			override {};

private:
	std::unique_ptr<EditorScene> m_Scene;
};