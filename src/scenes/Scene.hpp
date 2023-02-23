#pragma once

#include <stdint.h>
#include <vector>
#include <memory>

struct Event;

class Planet;
class Camera;
class SceneState;

struct SceneData
{
	std::vector<Planet>* Planets = nullptr;
	Planet** SelectedPlanet = nullptr;
	
	Camera* EditorCamera = nullptr;
	
	bool* ShowGrid = nullptr;
};

class Scene
{
public:
	virtual void OnEvent(Event& ev) = 0;
	virtual void OnInput()			= 0;
	virtual void OnUpdate(float ts)	= 0;
	virtual void OnRender()			= 0;

	virtual void SetState(std::unique_ptr<SceneState>&& state) = 0;

	virtual SceneData GetSceneData() = 0;
	virtual uint32_t GetFramebufferTextureID() const = 0;
};