#include "Scene.hpp"
#include "../OpenGL.hpp"
#include "../renderer/Camera.hpp"
#include "../objects/Planet.hpp"
#include "states/SceneState.hpp"

#include <memory>

class EditorScene : public Scene
{
public:
	EditorScene();
	~EditorScene();

	virtual void OnEvent(Event& ev) override;
	virtual void OnInput()			override;
	virtual void OnUpdate(float ts)	override;
	virtual void OnRender()			override;

	virtual void SetState(std::unique_ptr<SceneState>&& state) override;

	virtual SceneData GetSceneData() override;
	virtual uint32_t GetFramebufferTextureID() const override;

	void CancelState();

private:
	void CheckForPlanetSelect();
	void DrawGridPlane();

	std::unique_ptr<SceneState> m_ActiveState;

	std::vector<Planet> m_Planets;
	Planet* m_SelectedPlanet = nullptr;

	Camera m_Camera;

	std::unique_ptr<Framebuffer> m_FB;
	std::shared_ptr<Cubemap> m_SkyboxTex;

	float m_TS = 0.0f;
	bool  m_ShowGrid = true;

	friend class EditorLayer;
};