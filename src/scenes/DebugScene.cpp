#include "DebugScene.hpp"
#include "../Logger.hpp"
#include "../Event.hpp"
#include "../Application.hpp"
#include "../renderer/Renderer.hpp"

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

	m_VAO = std::make_shared<VertexArray>();

	VertexBuffer vbo(vertices, 6 * sizeof(float));
	std::unique_ptr<IndexBuffer> ibo = std::make_unique<IndexBuffer>(indices, 3);

	VertexBufferLayout layout;

	layout.Push<float>(2);
	m_VAO->AddBuffer(vbo, ibo, layout);
	m_VAO->Unbind();

	m_Shader = std::make_shared<Shader>("res/shaders/Triangle.vert", "res/shaders/Triangle.frag");
	m_Shader->Bind();

	Renderer::Init();

	m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, -3.0f));

	LOG_INFO("DebugScene initialized.");
}

DebugScene::~DebugScene()
{
	LOG_INFO("DebugScene destroyed.");
}

void DebugScene::OnEvent(Event& ev)
{
	if (ev.Type == Event::KeyPressed && ev.Key.Code == Key::K)
	{
		glm::vec3 pos = m_Camera.GetPosition();

		LOG_INFO("Camera position: ({}, {}, {})", pos.x, pos.y, pos.z);
	}
	else if (ev.Type == Event::WindowResized)
	{
		m_Camera.SetViewportSize(glm::vec2(ev.Size.Width, ev.Size.Height));

		LOG_INFO("Window resized to: {}x{}", ev.Size.Width, ev.Size.Height);
	}

	m_Camera.OnEvent(ev);
}

void DebugScene::OnInput()
{
}

void DebugScene::OnUpdate(float ts)
{
	m_FPS = 1.0f / ts;

	m_Camera.OnUpdate(ts);

	m_Shader->Bind();
	m_Shader->SetUniform4f("u_Color", m_TriangleColor);
}

void DebugScene::OnRender()
{
	m_FB->BindBuffer();

	Renderer::Clear();
	Renderer::ClearColor(glm::vec4(0.33f, 0.33f, 0.33f, 1.0f));
	Renderer::SceneBegin(m_Camera);
	
	if (m_ShouldBeRendered)
	{
		Renderer::SubmitIndexed(m_Shader, m_VAO,
			glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f)), glm::vec3(2.0f, 0.8f, 2.0f)));
	}

	m_FB->UnbindBuffer();
	m_FB->BindTexture(1);
}

void DebugScene::OnConfigRender()
{
	ImGui::Begin("Control panel", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	ImGui::Text("FPS: %.2f", m_FPS);
	ImGui::Separator();

	ImGui::Text("Triangle color picker");
	ImGui::ColorEdit4("Color", glm::value_ptr(m_TriangleColor));
	ImGui::Separator();
	
	ImGui::Text("Visibility");
	ImGui::Checkbox("Show triangle", &m_ShouldBeRendered);

	ImGui::End();
}

uint32_t DebugScene::GetFramebufferTextureID() const
{
	return m_FB->GetTextureID();
}
