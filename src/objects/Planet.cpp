#include "Planet.hpp"
#include "../TextureManager.hpp"

#include <imgui/imgui.h>
#include <imgui_dialog/ImGuiFileDialog.h>
#include <glm/gtc/type_ptr.hpp>

static uint32_t s_IdCounter = 1;

Planet::Planet(const glm::vec3& position)
{
	m_EntityID = s_IdCounter;
	m_Tag.reserve(24);
	m_Tag = std::string("Planet #") + std::to_string(m_EntityID);
	++s_IdCounter;

	m_Transform.Position = position;
}

void Planet::OnUpdate(float ts)
{
	m_Transform.Position += m_Physics.LinearVelocity * ts;
}

void Planet::SetPosition(const glm::vec3& position)
{
	m_Transform.Position = position;
}

void Planet::SetScale(const glm::vec3& scale)
{
	m_Transform.Scale = scale;
}

void Planet::SetRadius(float radius)
{
	m_Transform.Scale = glm::vec3(radius);
}

void Planet::SetRotation(const glm::vec3& rotation)
{
	m_Transform.Rotation = rotation;
}

void Planet::SetMass(float mass)
{
	m_Physics.Mass = mass;
}

void Planet::SetTextureID(int32_t id)
{
	m_Material.TextureID = id;
}

void Planet::SetRelative(Planet* other)
{
	m_EditorRelative = other;
}

void Planet::SetVelocity(const glm::vec3& velocity)
{
	m_Physics.LinearVelocity = velocity;
}

void Planet::AddVelocity(const glm::vec3& addVelocity)
{
	m_Physics.LinearVelocity += addVelocity;
}

void Planet::OnConfigRender()
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
	
	ImGui::EndChild();
}
