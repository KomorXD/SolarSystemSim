#pragma once

#include <stdint.h>
#include <vector>
#include <memory>

struct Event;
class SceneState;

class Scene
{
public:
	virtual void OnEvent(Event& ev) = 0;
	virtual void OnInput()			= 0;
	virtual void OnUpdate(float ts)	= 0;
	virtual void OnRender()			= 0;

	virtual void SetState(std::unique_ptr<SceneState>&& state) = 0;

	virtual uint32_t GetFramebufferTextureID() const = 0;
};