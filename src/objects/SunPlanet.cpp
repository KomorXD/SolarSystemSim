#include "SunPlanet.hpp"

#include <imgui/imgui.h>

SunPlanet::SunPlanet(const glm::vec3& position)
	: Planet(position)
{
}

void SunPlanet::OnConfigRender()
{
	ImGui::BeginChild("Sphere settings");

	ImGui::Text("Planet #%d", m_EntityID);
	ImGui::Indent(16.0f);
	ImGui::InputText("##Planet tag", m_Tag.data(), m_Tag.capacity());
	ImGui::Unindent(16.0f);
	ImGui::NewLine();

	m_Transform.OnImGuiRender();
	ImGui::NewLine();

	m_Physics.OnImGuiRender();
	ImGui::NewLine();

	m_Material.OnImGuiRender();
	ImGui::NewLine();

	m_Light.OnImGuiRender();
	ImGui::NewLine();

	ImGui::EndChild();
}
