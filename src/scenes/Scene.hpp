#pragma once

struct Event;

class Scene
{
public:
	virtual void OnEvent(Event& ev) = 0;
	virtual void OnInput()			= 0;
	virtual void OnUpdate(float ts)	= 0;
	virtual void OnRender()			= 0;
};