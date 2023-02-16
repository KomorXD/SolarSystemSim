#include "DebugScene.hpp"
#include "../Logger.hpp"
#include "../Event.hpp"
#include "../Application.hpp"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

DebugScene::DebugScene()
{
	float vertices[] =
	{
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.0f,  0.5f
	};

	uint32_t indices[] =
	{
		0, 1, 2
	};

	m_FB = std::make_unique<Framebuffer>();
	m_FB->AttachTexture(Application::GetInstance()->GetWindowSpec().Width, Application::GetInstance()->GetWindowSpec().Height);
	m_FB->UnbindBuffer();

	m_VAO = std::make_unique<VertexArray>();
	m_VBO = std::make_unique<VertexBuffer>(vertices, 6 * sizeof(float));
	m_IBO = std::make_unique<IndexBuffer>(indices, 3);

	VertexBufferLayout layout;

	layout.Push<float>(2);
	m_VAO->AddBuffer(*m_VBO, layout);

	m_Shader = std::make_shared<Shader>("res/shaders/Triangle.vert", "res/shaders/Triangle.frag");
	m_Shader->Bind();

	LOG_INFO("DebugScene initialized.");
}

DebugScene::~DebugScene()
{
	LOG_INFO("DebugScene destroyed.");
}

void DebugScene::OnEvent(Event& ev)
{
	if(ev.Type == Event::KeyPressed && ev.Key.Code == Key::A)
	{
		m_ShouldBeRendered = !m_ShouldBeRendered;

		LOG_INFO("Event caught, flag is now: {}", m_ShouldBeRendered ? "true" : "false");
	}
}

void DebugScene::OnInput()
{

}

void DebugScene::OnUpdate(float ts)
{
	m_TimePassedInSeconds += ts;

	if (m_TimePassedInSeconds > 1.0f)
	{
		m_TimePassedInSeconds = 0.0f;

		LOG_INFO("1 second passed, FPS: {}", 1.0f / ts);
		LOG_WARN("Mouse position: {}x{}", Input::GetMousePosition().x, Input::GetMousePosition().y);
	}

	m_Shader->Bind();
	m_Shader->SetUniform4f("u_Color", m_TriangleColor);
}

void DebugScene::OnRender()
{
	m_FB->BindBuffer();

	GLCall(glClearColor(0.3f, 0.3f, 0.3f, 1.0f));
	
	GLCall(glDrawElements(GL_TRIANGLES, m_IBO->GetCount(), GL_UNSIGNED_INT, nullptr));

	m_FB->UnbindBuffer();
	m_FB->BindTexture(1);
}

uint32_t DebugScene::GetFramebufferTextureID() const
{
	return m_FB->GetTextureID();
}
