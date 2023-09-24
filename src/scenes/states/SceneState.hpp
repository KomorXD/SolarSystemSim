#pragma once

struct Event;
class Camera;

class SceneState
{
public:
	virtual void OnEvent(Event& ev) = 0;
	virtual void OnUpdate(float ts) = 0;
	virtual void OnTick()			= 0;
	virtual void OnRender()			= 0;
};