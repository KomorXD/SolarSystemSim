#include "Scene.hpp"
#include "../OpenGL.hpp"
#include "../renderer/Camera.hpp"
#include "../objects/Sun.hpp"
#include "states/SceneState.hpp"

#include <memory>

class EditorScene : public Scene
{
public:
	EditorScene();
	EditorScene(const EditorScene& other);
	EditorScene(EditorScene&& other) noexcept;
	~EditorScene();

	EditorScene& operator= (EditorScene&& other) noexcept
	{
		return Assign(std::move(other));
	}

	virtual void OnEvent(Event& ev) override;
	virtual void OnInput()			override;
	virtual void OnUpdate(float ts)	override;
	virtual void OnTick()			override;
	virtual void OnRender()			override;

	virtual void SetState(std::unique_ptr<SceneState>&& state) override;
	virtual uint32_t GetFramebufferTextureID() const override;

	inline std::vector<std::unique_ptr<Planet>>& GetPlanetsRef() { return m_Planets; }
	inline Planet* SelectedPlanet() { return m_SelectedPlanet; }

	void SetViewportOffset(const glm::vec2& offset);
	void CancelState();

private:
	EditorScene& Assign(EditorScene&& other) noexcept;
	void CheckForPlanetSelect();
	void DrawGridPlane();

	std::string m_SceneName = "New scene";
	std::string m_ScenePath = "";

	std::unique_ptr<SceneState> m_ActiveState;

	std::vector<std::unique_ptr<Planet>> m_Planets;
	Planet* m_SelectedPlanet = nullptr;

	Camera m_Camera;

	std::unique_ptr<Framebuffer> m_FB;
	std::unique_ptr<MultisampledFramebuffer> m_MFB;
	std::shared_ptr<Cubemap> m_SkyboxTex;
	glm::vec2 m_ViewportOffset;

	float m_TS = 0.0f;
	bool  m_RenderGrid = true;
	bool  m_RenderSkybox = false;
	bool  m_LockFocusOnPlanet = false;
	
	static inline float TS_MULTIPLIER = 1.0f;

	friend class EditorLayer;
	friend class SimulationLayer;
	friend class SceneSerializer;
};