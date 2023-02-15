#include "Scene.hpp"
#include "../OpenGL.hpp"

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

private:
	bool m_ShouldBeRendered = false;
	float m_TimePassedInSeconds = 0.0f;

	std::unique_ptr<VertexArray>  m_VAO;
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<IndexBuffer>  m_IBO;
	std::shared_ptr<Shader>		  m_Shader;
};