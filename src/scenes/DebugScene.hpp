#include "Scene.hpp"
#include "../OpenGL.hpp"
#include "../renderer/Camera.hpp"

#include <memory>

class DebugScene : public Scene
{
public:
	DebugScene();
	~DebugScene();

	virtual void OnEvent(Event& ev) override;
	virtual void OnInput()			override;
	virtual void OnUpdate(float ts)	override;
	virtual void OnRender()			override;
	virtual void OnConfigRender()	override;

	virtual uint32_t GetFramebufferTextureID() const override;

private:
	void DrawGridPlane();

	float m_TS = 0.0f;
	bool m_ShowGrid = true;

	Camera m_Camera;

	std::unique_ptr<Framebuffer> m_FB;
	std::shared_ptr<Cubemap> m_SkyboxTex;

	glm::vec4 m_SphereColor{ 1.0f, 0.0f, 0.0f, 1.0f };
};