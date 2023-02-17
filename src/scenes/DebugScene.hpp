#include "Scene.hpp"
#include "../OpenGL.hpp"
#include "../renderer/Camera.hpp"

#include <glm/gtc/type_ptr.hpp>
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
	bool m_ShouldBeRendered = true;
	float m_FPS = 1.0f / 60.0f;

	Camera m_Camera;

	glm::vec4 m_TriangleColor{ 1.0f, 0.0f, 0.0f, 1.0f };

	std::unique_ptr<Framebuffer> m_FB;
	std::shared_ptr<VertexArray> m_VAO;
	std::shared_ptr<Shader>		 m_Shader;
};