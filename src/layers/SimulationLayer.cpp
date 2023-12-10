#include "../scenes/states/EditorSceneStates.hpp"
#include "SimulationLayer.hpp"
#include "../Simulator.hpp"
#include "../Application.hpp"
#include "../scenes/EditorScene.hpp"
#include "../TextureManager.hpp"
#include "../objects/Sun.hpp"

#include <imgui/imgui.h>

SimulationLayer::SimulationLayer(std::unique_ptr<EditorScene>& scene)
{
	WindowSpec spec = Application::GetInstance()->GetWindowSpec();
	Event dummyEv{};
	dummyEv.Type = Event::WindowResized;
	dummyEv.Size.Width = spec.Width;
	dummyEv.Size.Height = spec.Height - m_ControlBarHeight;

	m_Scene = std::make_unique<EditorScene>(*scene);
	m_Scene->OnEvent(dummyEv);
	m_Scene->SetViewportOffset({ 0.0f, m_ControlBarHeight });
}

SimulationLayer::~SimulationLayer()
{
}

void SimulationLayer::OnEvent(Event& ev)
{
	if (ev.Type == Event::WindowResized && ev.Size.Width != 0 && ev.Size.Height != 0)
	{
		ev.Size.Height -= m_ControlBarHeight;
		m_Scene->OnEvent(ev);
		m_Scene->SetViewportOffset({ 0.0f, 0.0f });
		m_Scene->SetViewportOffset({ 0.0f, m_ControlBarHeight });

		return;
	}

	if (m_IsViewportFocused)
	{
		if (ev.Type == Event::KeyPressed && ev.Key.Code == Key::G 
			&& m_Scene->SelectedPlanet() && m_Scene->m_ActiveState == nullptr)
		{
			m_Scene->SetState(std::make_unique<FollowingPlanetState>(m_Scene.get(), &m_Scene->m_Camera, m_Scene->SelectedPlanet()));
		}

		m_Scene->OnEvent(ev);
	}
}

void SimulationLayer::OnInput()
{
	if (m_IsViewportFocused)
	{
		m_Scene->OnInput();
	}
}

void SimulationLayer::OnUpdate(float ts)
{
	if (m_IsRunning)
	{
		m_SimulationTimePassed += ts * Application::TPS_MULTIPLIER;
		m_RealTimePassed += ts;
	}
		
	m_Scene->OnUpdate(ts);
}

void SimulationLayer::OnTick()
{
	if (m_IsRunning)
	{
		m_Scene->OnTick();
	}
}

void SimulationLayer::OnImGuiRender()
{
	RenderViewport();
	
	if (m_Scene->m_SelectedPlanet)
	{
		m_Scene->m_SelectedPlanet->OnSimDataRender();
	}
	
	RenderControlBar();
}

void SimulationLayer::RenderControlBar()
{
	WindowSpec windowSpec = Application::GetInstance()->GetWindowSpec();

	ImGui::SetNextWindowPos({ 0.0f, windowSpec.Height - m_ControlBarHeight });
	ImGui::SetNextWindowSize({ (float)windowSpec.Width, m_ControlBarHeight });
	ImGui::Begin("##ControlBar", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

	TextureInfo playIcon = TextureManager::GetTexture(TextureManager::PLAY_ICON).value_or(
		TextureManager::GetTexture(TextureManager::DEFAULT_ALBEDO).value()
	);
	ImGui::PushID(1);
	if (ImGui::ImageButton((ImTextureID)TextureManager::GetAtlasTextureID(), { 16.0f, 16.0f },
		{ playIcon.UV.x, playIcon.UV.y }, { playIcon.UV.x + playIcon.Size.x, playIcon.UV.y + playIcon.Size.y }))
	{
		m_IsRunning = true;
	}
	ImGui::PopID();
	ImGui::SameLine();

	TextureInfo pauseIcon = TextureManager::GetTexture(TextureManager::PAUSE_ICON).value_or(
		TextureManager::GetTexture(TextureManager::DEFAULT_ALBEDO).value()
	);
	ImGui::PushID(2);
	if (ImGui::ImageButton((ImTextureID)TextureManager::GetAtlasTextureID(), { 16.0f, 16.0f },
		{ pauseIcon.UV.x, pauseIcon.UV.y }, { pauseIcon.UV.x + pauseIcon.Size.x, pauseIcon.UV.y + pauseIcon.Size.y }))
	{
		m_IsRunning = false;
	}
	ImGui::PopID();
	ImGui::SameLine();

	TextureInfo stopIcon = TextureManager::GetTexture(TextureManager::STOP_ICON).value_or(
		TextureManager::GetTexture(TextureManager::DEFAULT_ALBEDO).value()
	);
	ImGui::PushID(3);
	if (ImGui::ImageButton((ImTextureID)TextureManager::GetAtlasTextureID(), { 16.0f, 16.0f },
		{ stopIcon.UV.x, stopIcon.UV.y }, { stopIcon.UV.x + stopIcon.Size.x, stopIcon.UV.y + stopIcon.Size.y }))
	{
		Application::GetInstance()->PopLayer();
		ImGui::PopID();
		ImGui::End();

		return;
	}
	ImGui::PopID();
	ImGui::SameLine();

	static bool s_ShowSettings = false;

	TextureInfo settingsIcon = TextureManager::GetTexture(TextureManager::SETTINGS_ICON).value_or(
		TextureManager::GetTexture(TextureManager::DEFAULT_ALBEDO).value()
	);
	ImGui::PushID(4);
	if (ImGui::ImageButton((ImTextureID)TextureManager::GetAtlasTextureID(), { 16.0f, 16.0f },
		{ settingsIcon.UV.x, settingsIcon.UV.y }, { settingsIcon.UV.x + settingsIcon.Size.x, settingsIcon.UV.y + settingsIcon.Size.y }))
	{
		s_ShowSettings = !s_ShowSettings;
	}

	if (s_ShowSettings)
	{
		ImGui::SetNextWindowSize({ 512.0f, 312.0f }, ImGuiCond_FirstUseEver);
		ImGui::Begin("Simulation settings");
		ImGui::PrettyDragFloat("G Constant Multiplier", &SimPhysics::G_CONSTANT_MULTIPLIER, 0.0f, 0.0f, 200.0f);
		ImGui::PrettyDragFloat("Time scale (1.0 = 1 day)", &Application::TPS_MULTIPLIER, 0.0f, 365.0f, 200.0f);
		ImGui::NewLine();
		ImGui::Text("Simulation time passed: %.10f days", m_SimulationTimePassed / 365.0f);
		ImGui::Text("Real time passed: %.10f seconds", m_RealTimePassed);

		ImGui::End();
	}

	ImGui::PopID();
	ImGui::End();
}

void SimulationLayer::RenderViewport()
{
	WindowSpec windowSpec = Application::GetInstance()->GetWindowSpec();

	ImGui::SetNextWindowPos({ 0.0f, 0.0f });
	ImGui::SetNextWindowSize({ (float)windowSpec.Width, (float)windowSpec.Height - m_ControlBarHeight });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
	ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);

	m_Scene->OnRender();
	m_IsViewportFocused = ImGui::IsWindowHovered();
	ImGui::Image((ImTextureID)m_Scene->GetFramebufferTextureID(), ImGui::GetContentRegionAvail(), { 0.0f, 1.0f }, { 1.0f, 0.0f });

	ImGui::End();
	ImGui::PopStyleVar();
}
