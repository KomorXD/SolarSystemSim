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

	float m_TimeScale			 = 1.0f;
	float m_SimulationTimePassed = 0.0f;
	float m_RealTimePassed		 = 0.0f;
	
	float m_ControlBarHeight = 40.0f;
	bool m_IsRunning = false;
};