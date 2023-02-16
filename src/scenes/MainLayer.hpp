#pragma once

#include "Layer.hpp"
#include "DebugScene.hpp"

#include <memory>

class MainLayer : public Layer
{
public:
	MainLayer();

	virtual void OnEvent(Event& ev) override;
	virtual void OnInput()			override;
	virtual void OnUpdate(float ts) override;
	virtual void OnImGuiRender()	override;

private:
	std::unique_ptr<DebugScene> m_Scene;
};