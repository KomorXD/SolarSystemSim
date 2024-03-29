#include "EditorLayer.hpp"
#include "SimulationLayer.hpp"
#include "../scenes/states/EditorSceneStates.hpp"
#include "../Application.hpp"
#include "../Logger.hpp"
#include "../random_utils/Math.hpp"
#include "../random_utils/SceneSerializer.hpp"
#include "../renderer/Renderer.hpp"
#include "../objects/Sun.hpp"
#include "../TextureManager.hpp"

#include <imgui/imgui.h>
#include <imgui_dialog/ImGuiFileDialog.h>
#include <imgui/ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>

EditorLayer::EditorLayer()
{
	WindowSpec spec = Application::GetInstance()->GetWindowSpec();
	Event dummyEv{};
	dummyEv.Type = Event::WindowResized;
	dummyEv.Size.Width = (uint32_t)(spec.Width * 0.6f);
	dummyEv.Size.Height = spec.Height - m_TopbarHeight;
	
	m_Scene = std::make_unique<EditorScene>();
	m_Scene->OnEvent(dummyEv);
	m_Scene->SetViewportOffset({ spec.Width * 0.2f, 0.0f });
}

void EditorLayer::OnEvent(Event& ev)
{
	WindowSpec spec = Application::GetInstance()->GetWindowSpec();

	if (ev.Type == Event::WindowResized && ev.Size.Width != 0 && ev.Size.Height != 0)
	{
		ev.Size.Width = (uint32_t)(ev.Size.Width * 0.6f);
		ev.Size.Height -= m_TopbarHeight;
		m_Scene->OnEvent(ev);
		m_Scene->SetViewportOffset({ spec.Width * 0.2f, 0.0f });

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
	m_Scene->OnUpdate(ts);
}

void EditorLayer::OnTick()
{
	;
}

void EditorLayer::OnImGuiRender()
{
	RenderScenePanel();
	RenderViewport();
	RenderTopbar();
	RenderControlPanel();
}

void EditorLayer::OnAttach()
{
	WindowSpec spec = Application::GetInstance()->GetWindowSpec();
	Event dummyEv{};
	dummyEv.Type = Event::WindowResized;
	dummyEv.Size.Width = (uint32_t)(spec.Width * 0.6f);
	dummyEv.Size.Height = spec.Height - m_TopbarHeight;

	m_Scene->OnEvent(dummyEv);
}

void EditorLayer::RenderScenePanel()
{
	WindowSpec windowSpec = Application::GetInstance()->GetWindowSpec();
						
	ImGui::SetNextWindowPos({ 0.0f, 0.0f });
	ImGui::SetNextWindowSize({ windowSpec.Width * 0.2f, windowSpec.Height * 1.0f });

	char buf[24];
	strcpy_s(buf, 24, m_Scene->m_SceneName.data());

	ImGui::Begin("Scene panel", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::InputText("##SceneName", buf, 24);

	m_Scene->m_SceneName = buf;

	if (ImGui::Button("Start simulation"))
	{
		Application::GetInstance()->PushLayer(std::make_unique<SimulationLayer>(m_Scene));
		ImGui::End();

		return;
	}

	if (ImGui::Button("Save scene"))
	{
		SceneSerializer::SaveScene(*m_Scene);
	}

	ImGui::SameLine();

	if (ImGui::Button("Load scene"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("ChooseSceneKey", "Choose scene file",
			".sscene, All files (*.*){.*}", ".", 1, nullptr, ImGuiFileDialogFlags_CaseInsensitiveExtention);
	}

	if (ImGuiFileDialog::Instance()->Display("ChooseSceneKey", ImGuiWindowFlags_NoCollapse, ImVec2(600.0f, 500.0f)))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::optional<EditorScene> newScene = SceneSerializer::LoadScene(filePathName);

			if (newScene.has_value())
			{
				WindowSpec spec = Application::GetInstance()->GetWindowSpec();
				Event dummyEv{};
				dummyEv.Type = Event::WindowResized;
				dummyEv.Size.Width = (uint32_t)(spec.Width * 0.6f);
				dummyEv.Size.Height = spec.Height - m_TopbarHeight;

				m_Scene = std::make_unique<EditorScene>(std::move(newScene.value()));
				m_Scene->OnEvent(dummyEv);
				m_Scene->SetViewportOffset({ spec.Width * 0.2f, 0.0f });

				LOG_INFO("Loaded new scene");
			}
			else
			{
				LOG_ERROR("Could not fetch scene");
			}
		}

		ImGuiFileDialog::Instance()->Close();
	}

	ImGui::NewLine();

	ImVec2 avSpace = ImGui::GetContentRegionAvail();
	Planet*& selectedPlanet = m_Scene->m_SelectedPlanet;
	ImGui::Text("Spawned objects");
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	ImGui::BeginChild("Spawned objects", ImVec2(avSpace.x, avSpace.y / 3.0f), true);
	
	for (size_t i = 0; i < m_Scene->m_Planets.size(); i++)
	{
		Planet& planet = *m_Scene->m_Planets[i];

		ImGui::PushID(i);
		if (ImGui::Selectable(planet.GetTag().c_str(), &planet == selectedPlanet))
		{
			selectedPlanet = &planet;
		}
		ImGui::PopID();
	}

	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::NewLine();
	ImGui::Separator();
	ImGui::NewLine();

	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Camera& cam = m_Scene->m_Camera;

		ImGui::Indent(16.0f);
		ImGui::PrettyDragFloat3("Position", glm::value_ptr(cam.m_Position));
		ImGui::PrettyDragFloat("Near clip", &cam.m_NearClip, 0.0f, cam.m_FarClip);
		ImGui::PrettyDragFloat("Far clip", &cam.m_FarClip, cam.m_NearClip, 1000.0f);
		ImGui::PrettyDragFloat("FOV", &cam.m_FOV, 0.0f, 113.0f);
		ImGui::Unindent(16.0f);
		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();
	}

#ifdef CONF_DEBUG
	ImGui::SameLine();

	if (ImGui::Button("Reload shaders"))
	{
		Renderer::ReloadShaders();
	}
#endif

	ImGui::NewLine();

	ImGui::End();
}

void EditorLayer::RenderViewport()
{
	WindowSpec windowSpec = Application::GetInstance()->GetWindowSpec();

	m_Scene->OnRender();

	ImGui::SetNextWindowPos({ windowSpec.Width * 0.2f, m_TopbarHeight });
	ImGui::SetNextWindowSize({ windowSpec.Width * 0.6f, windowSpec.Height * 1.0f - m_TopbarHeight });
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
	ImGuizmo::SetRect(spec.Width * 0.2f, m_TopbarHeight, spec.Width * 0.6f, spec.Height - m_TopbarHeight);

	const glm::mat4& cameraProj = editorCamera.GetProjection();
	glm::mat4 cameraView = editorCamera.GetViewMatrix();
	glm::mat4 planetTransform = selectedPlanet->GetTransform().Matrix();

	bool doSnap = Input::IsKeyPressed(Key::LeftControl);
	float snapStep = m_GizmoMode == ImGuizmo::ROTATE ? 45.0f : 0.5f;
	float snapArr[3]{ snapStep, snapStep, snapStep };
	
	ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProj),
		ImGuizmo::OPERATION(m_GizmoMode), ImGuizmo::MODE(m_GizmoCoords), glm::value_ptr(planetTransform),
		nullptr, doSnap ? snapArr : nullptr);
	
	m_Scene->m_LockFocusOnPlanet = ImGuizmo::IsOver();

	if (ImGuizmo::IsUsing())
	{
		glm::vec3 translation{};
		glm::vec3 rotation{};
		glm::vec3 scale{};

		Math::TransformDecompose(planetTransform, translation, rotation, scale);

		glm::vec3 deltaRotation = rotation - selectedPlanet->GetTransform().Rotation;

		selectedPlanet->GetTransform().Position = translation;
		selectedPlanet->GetTransform().Rotation = selectedPlanet->GetTransform().Rotation + deltaRotation;
		selectedPlanet->GetTransform().Scale	= scale;
	}
}

void EditorLayer::RenderTopbar()
{
	WindowSpec windowSpec = Application::GetInstance()->GetWindowSpec();

	ImGui::SetNextWindowPos({ windowSpec.Width * 0.2f, 0.0f });
	ImGui::SetNextWindowSize({ windowSpec.Width * 0.6f, m_TopbarHeight });
	
	ImGui::Begin("Topbar", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	ImGui::Columns(5);
	ImGui::SetColumnWidth(0, 72.0f);
	ImGui::SetColumnWidth(1, 96.0f);
	ImGui::SetColumnWidth(2, 144.0f);
	ImGui::SetColumnWidth(3, 86.0f);
	ImGui::SetColumnWidth(4, 86.0f);

	Camera& editorCam = m_Scene->m_Camera;
	Planet* selectedPlanet = m_Scene->m_SelectedPlanet;

	std::optional<TextureInfo> texture = TextureManager::GetTexture(TextureManager::NEW_PLANET_ICON);
	TextureInfo texInfo = texture.value_or(TextureManager::GetTexture(TextureManager::DEFAULT_ALBEDO).value());
	ImGui::PushID(1);
	if (ImGui::ImageButton((ImTextureID)TextureManager::GetAtlasTextureID(), ImVec2(16.0f, 16.0f),
		ImVec2(texInfo.UV.x, texInfo.UV.y), ImVec2(texInfo.UV.x + texInfo.Size.x, texInfo.UV.y + texInfo.Size.y)))
	{
		m_Scene->m_SelectedPlanet = m_Scene->m_Planets.emplace_back(std::make_unique<Planet>()).get();
	}
	ImGui::PopID();
	ImGui::SameLine();

	texture = TextureManager::GetTexture(TextureManager::NEW_SUN_ICON);
	texInfo = texture.value_or(TextureManager::GetTexture(TextureManager::DEFAULT_ALBEDO).value());
	ImGui::PushID(2);
	if (ImGui::ImageButton((ImTextureID)TextureManager::GetAtlasTextureID(), ImVec2(16.0f, 16.0f),
		ImVec2(texInfo.UV.x, texInfo.UV.y), ImVec2(texInfo.UV.x + texInfo.Size.x, texInfo.UV.y + texInfo.Size.y)))
	{
		m_Scene->m_SelectedPlanet = m_Scene->m_Planets.emplace_back(std::make_unique<Sun>()).get();
	}
	ImGui::PopID();
	ImGui::NextColumn();
	
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.25f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
	if (ImGui::Button("X", ImVec2(22.0f, 22.0f)))
	{
		glm::vec3 rotatePoint = selectedPlanet ? selectedPlanet->GetTransform().Position : glm::vec3(0.0f);
		float distance = glm::distance(editorCam.GetPosition(), rotatePoint);
	
		m_Scene->SetState(std::make_unique<InterpolateViewState>(m_Scene.get(), &m_Scene->m_Camera,
			rotatePoint + glm::vec3(distance, 0.0f, 0.0f), glm::radians(0.0f), glm::radians(-90.0f)));
	}
	
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
	if (ImGui::Button("Y", ImVec2(22.0f, 22.0f)))
	{
		glm::vec3 rotatePoint = selectedPlanet ? selectedPlanet->GetTransform().Position : glm::vec3(0.0f);
		float distance = glm::distance(editorCam.GetPosition(), rotatePoint);
	
		m_Scene->SetState(std::make_unique<InterpolateViewState>(m_Scene.get(), &m_Scene->m_Camera,
			rotatePoint + glm::vec3(0.0f, distance, 0.0f), glm::radians(90.0f), glm::radians(0.0f)));
	}
	
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
	if (ImGui::Button("Z", ImVec2(22.0f, 22.0f)))
	{
		glm::vec3 rotatePoint = selectedPlanet ? selectedPlanet->GetTransform().Position : glm::vec3(0.0f);
		float distance = glm::distance(editorCam.GetPosition(), rotatePoint);
	
		m_Scene->SetState(std::make_unique<InterpolateViewState>(m_Scene.get(), &m_Scene->m_Camera,
			rotatePoint + glm::vec3(0.0f, 0.0f, distance), glm::radians(0.0f), glm::radians(0.0f)));
	}
	
	ImGui::PopStyleColor(3);
	ImGui::NextColumn();
	
	ImGui::SetNextItemWidth(128.0f);
	if (ImGui::BeginCombo("##Coordinate space", 
		(std::string("Space: ") + (m_GizmoCoords == ImGuizmo::WORLD ? "World" : "Local")).c_str()))
	{
		if (ImGui::Selectable("Local", m_GizmoCoords == ImGuizmo::LOCAL))
		{
			m_GizmoCoords = ImGuizmo::LOCAL;
		}
	
		if (ImGui::Selectable("World", m_GizmoCoords == ImGuizmo::WORLD))
		{
			m_GizmoCoords = ImGuizmo::WORLD;
		}
	
		ImGui::EndCombo();
	}

	ImGui::NextColumn();
	ImGui::Checkbox("Grid", &m_Scene->m_RenderGrid);

	ImGui::NextColumn();
	ImGui::Checkbox("Skybox", &m_Scene->m_RenderSkybox);
	
	ImGui::Columns(1);
	ImGui::End();
}

void EditorLayer::RenderControlPanel()
{
	WindowSpec windowSpec = Application::GetInstance()->GetWindowSpec();

	ImGui::SetNextWindowPos({ windowSpec.Width * 0.8f, 0.0f });
	ImGui::SetNextWindowSize({ windowSpec.Width * 0.2f, windowSpec.Height * 1.0f });

	glm::vec3 cameraPos = m_Scene->m_Camera.GetPosition();
	
	ImGui::Begin("Object properties", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	
	if (m_Scene->m_SelectedPlanet != nullptr)
	{
		ImGui::NewLine();
		RenderPlanetsCombo();

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		RenderEntityData();
	}

	ImGui::End();
}

void EditorLayer::RenderPlanetsCombo()
{
	Planet* selectedPlanet = m_Scene->m_SelectedPlanet;

	if (ImGui::BeginCombo("Relative planet",
		selectedPlanet->GetRelativePlanet() != nullptr ? selectedPlanet->GetRelativePlanet()->GetTag().c_str() : "None"))
	{

		if (ImGui::Selectable("None", selectedPlanet->GetRelativePlanet() == nullptr))
		{
			selectedPlanet->SetRelativePlanet(nullptr);
		}

		for (size_t i = 0; i < m_Scene->m_Planets.size(); i++)
		{
			Planet& planet = *m_Scene->m_Planets[i];

			if (&planet == selectedPlanet)
			{
				continue;
			}

			ImGui::PushID(i);
			if (ImGui::Selectable(planet.GetTag().c_str(), &planet == selectedPlanet->GetRelativePlanet()))
			{
				selectedPlanet->SetRelativePlanet(&planet);
			}
			ImGui::PopID();
		}

		ImGui::EndCombo();
	}
}

void EditorLayer::RenderEntityData()
{
	if (m_Scene->m_SelectedPlanet)
	{
		m_Scene->m_SelectedPlanet->OnConfigRender();
	}
}
