#include "Scene.hpp"
#include "../OpenGL.hpp"
#include "../renderer/Camera.hpp"
#include "../objects/SunPlanet.hpp"
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
	virtual void OnTick()			override;
	virtual void OnRender()			override;

	virtual void SetState(std::unique_ptr<SceneState>&& state) override;
	
	virtual uint32_t GetFramebufferTextureID() const override;

	inline std::vector<Planet>& GetPlanetsRef() { return m_Planets; }
	inline std::vector<Planet> GetPlanetsCopy() const { return m_Planets; }

	void CancelState();

private:
	void CheckForPlanetSelect();
	void DrawGridPlane();

	std::unique_ptr<SceneState> m_ActiveState;

	std::vector<Planet> m_Planets;
	std::vector<SunPlanet> m_Suns;
	Planet* m_SelectedPlanet = nullptr;

	Camera m_Camera;

	std::unique_ptr<Framebuffer> m_FB;
	std::unique_ptr<MultisampledFramebuffer> m_MFB;
	
	std::shared_ptr<Cubemap> m_SkyboxTex;

	float m_TS = 0.0f;
	bool  m_RenderGrid = true;
	bool  m_RenderSkybox = false;
	bool  m_LockFocusOnPlanet = false;
	bool  m_Simulate = false;
	
	static inline float TS_MULTIPLIER = 1.0f;

	friend class EditorLayer;
};