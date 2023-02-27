#include "EditorLayer.hpp"
#include "../scenes/states/EditorSceneStates.hpp"
#include "../Application.hpp"
#include "../Logger.hpp"
#include "../math/Math.hpp"

#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>

EditorLayer::EditorLayer()
{
	m_Scene = std::make_unique<EditorScene>();

	WindowSpec spec = Application::GetInstance()->GetWindowSpec();
	Event dummyEv{};

	dummyEv.Type = Event::WindowResized;
	dummyEv.Size.Width = (uint32_t)(spec.Width * 0.8f);
	dummyEv.Size.Height = spec.Height;
	
	m_Scene->OnEvent(dummyEv);
}

void EditorLayer::OnEvent(Event& ev)
{
	WindowSpec spec = Application::GetInstance()->GetWindowSpec();

	if (ev.Type == Event::WindowResized)
	{
		ev.Size.Width = (uint32_t)(ev.Size.Width * 0.8f);
		m_Scene->OnEvent(ev);

		return;
	}

	if (m_IsViewportFocused)
	{
		if (ev.Type == Event::KeyPressed)
		{
			switch (ev.Key.Code)
			{
			case Key::Q:	m_GizmoMode = -1;				   return;
			case Key::W:	m_GizmoMode = ImGuizmo::TRANSLATE; return;
			case Key::E:	m_GizmoMode = ImGuizmo::ROTATE;    return;
			case Key::R:	m_GizmoMode = ImGuizmo::SCALE;	   return;
			default: break;
			}
		}

		m_Scene->OnEvent(ev);
	}
}

void EditorLayer::OnInput()
{
	if (m_IsViewportFocused)
	{
		m_Scene->OnInput();
	}
}

void EditorLayer::OnUpdate(float ts)
{
	if (m_IsViewportFocused || true)
	{
		m_Scene->OnUpdate(ts);
	}
}

void EditorLayer::OnImGuiRender()
{
	RenderViewport();
	RenderControlPanel();
}

void EditorLayer::RenderViewport()
{
	WindowSpec windowSpec = Application::GetInstance()->GetWindowSpec();

	m_Scene->OnRender();

	ImGui::SetNextWindowPos({ 0.0f, 0.0f });
	ImGui::SetNextWindowSize({ windowSpec.Width * 0.8f, windowSpec.Height * 1.0f });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
	ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

	m_IsViewportFocused = ImGui::IsWindowHovered();
	ImGui::Image((ImTextureID)m_Scene->GetFramebufferTextureID(), ImGui::GetContentRegionAvail(), { 0.0f, 1.0f }, { 1.0f, 0.0f });

	RenderImGuizmo();

	ImGui::End();
	ImGui::PopStyleVar();
}

void EditorLayer::RenderImGuizmo()
{
	if (!m_Scene->m_SelectedPlanet || m_GizmoMode == -1)
	{
		return;
	}

	Planet* selectedPlanet = m_Scene->m_SelectedPlanet;
	Camera& editorCamera = m_Scene->m_Camera;

	WindowSpec spec = Application::GetInstance()->GetWindowSpec();

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(0.0f, 0.0f, spec.Width * 0.8f, spec.Height * 1.0f);

	const glm::mat4& cameraProj = editorCamera.GetProjection();
	glm::mat4 cameraView = editorCamera.GetViewMatrix();
	glm::mat4 planetTransform = selectedPlanet->GetTransform();

	bool doSnap = Input::IsKeyPressed(Key::LeftControl);
	float snapStep = m_GizmoMode == ImGuizmo::ROTATE ? 45.0f : 0.5f;
	float snapArr[3]{ snapStep, snapStep, snapStep };
	
	ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProj),
		ImGuizmo::OPERATION(m_GizmoMode), ImGuizmo::WORLD, glm::value_ptr(planetTransform),
		nullptr, doSnap ? snapArr : nullptr);
	
	if (ImGuizmo::IsUsing())
	{
		glm::vec3 translation{};
		glm::vec3 rotation{};
		glm::vec3 scale{};

		Math::TransformDecompose(planetTransform, translation, rotation, scale);

		glm::vec3 deltaRotation = rotation - selectedPlanet->GetRotation();

		selectedPlanet->SetPosition(translation);
		selectedPlanet->SetRotation(selectedPlanet->GetRotation() + deltaRotation);
		selectedPlanet->SetScale(scale);
	}
}

void EditorLayer::RenderControlPanel()
{
	WindowSpec windowSpec = Application::GetInstance()->GetWindowSpec();

	ImGui::SetNextWindowPos({ windowSpec.Width * 0.8f, 0.0f });
	ImGui::SetNextWindowSize({ windowSpec.Width * 0.2f, windowSpec.Height * 1.0f });

	glm::vec3 cameraPos = m_Scene->m_Camera.GetPosition();
	
	ImGui::Begin("Control panel", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	
	ImGui::NewLine();
	ImGui::Text("Camera position: [%.2f %.2f %.2f]", cameraPos.x, cameraPos.y, cameraPos.z);
	ImGui::NewLine();
	ImGui::Separator();
	ImGui::NewLine();
	
	if (ImGui::Button("New planet"))
	{
		m_Scene->m_SelectedPlanet = &m_Scene->m_Planets.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f));
		m_Scene->SetState(std::make_unique<NewPlanetState>(m_Scene.get(), m_Scene->m_SelectedPlanet));
	}
	
	ImGui::Checkbox("Show grid", &m_Scene->m_ShowGrid);
	ImGui::NewLine();
	ImGui::Separator();
	ImGui::NewLine();
	
	if (ImGui::Button("X view"))
	{
		float distance = glm::length(m_Scene->m_Camera.GetPosition());
	
		m_Scene->SetState(std::make_unique<InterpolateViewState>(m_Scene.get(), &m_Scene->m_Camera,
			glm::vec3(distance, 0.0f, 0.0f), glm::radians(0.0f), glm::radians(-90.0f)));
	}
	
	ImGui::SameLine();
	
	if (ImGui::Button("Y view"))
	{
		float distance = glm::length(m_Scene->m_Camera.GetPosition());

		m_Scene->SetState(std::make_unique<InterpolateViewState>(m_Scene.get(), &m_Scene->m_Camera,
			glm::vec3(0.0f, distance, 0.0f), glm::radians(90.0f), glm::radians(0.0f)));
	}
	
	ImGui::SameLine();
	
	if (ImGui::Button("Z view"))
	{
		float distance = glm::length(m_Scene->m_Camera.GetPosition());

		m_Scene->SetState(std::make_unique<InterpolateViewState>(m_Scene.get(), &m_Scene->m_Camera,
			glm::vec3(0.0f, 0.0f, distance), glm::radians(0.0f), glm::radians(0.0f)));
	}
	
	ImGui::NewLine();
	ImGui::Separator();
	ImGui::NewLine();

	RenderEntityData();

	ImGui::End();
}

void EditorLayer::RenderEntityData()
{
	if (m_Scene->m_SelectedPlanet)
	{
		m_Scene->m_SelectedPlanet->OnConfigRender();
	}
}
