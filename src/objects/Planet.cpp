#include "Planet.hpp"
#include "../TextureManager.hpp"

#include <imgui/imgui.h>
#include <imgui_dialog/ImGuiFileDialog.h>
#include <glm/gtc/type_ptr.hpp>

static uint32_t s_IdCounter = 1;

PlanetaryObject::PlanetaryObject(const glm::vec3& position)
{
	m_EntityID = s_IdCounter;
	m_Tag.reserve(24);
	m_Tag = std::string("Planet #") + std::to_string(m_EntityID);
	++s_IdCounter;

	m_Transform.Position = position;
}

void PlanetaryObject::OnUpdate(float ts)
{
	m_Transform.Position += m_Physics.LinearVelocity * ts;
}

void PlanetaryObject::Move(const glm::vec3& offset)
{
	m_Transform.Position += offset;
}

void PlanetaryObject::SetPosition(const glm::vec3& position)
{
	m_Transform.Position = position;
}

void PlanetaryObject::SetScale(const glm::vec3& scale)
{
	m_Transform.Scale = scale;
}

void PlanetaryObject::SetRadius(float radius)
{
	m_Transform.Scale = glm::vec3(radius);
}

void PlanetaryObject::SetRotation(const glm::vec3& rotation)
{
	m_Transform.Rotation = rotation;
}

void PlanetaryObject::SetMass(float mass)
{
	m_Physics.Mass = mass;
}

void PlanetaryObject::SetTextureID(int32_t id)
{
	m_Material.TextureID = id;
}

void PlanetaryObject::SetVelocity(const glm::vec3& velocity)
{
	m_Physics.LinearVelocity = velocity;
}

void PlanetaryObject::AddVelocity(const glm::vec3& addVelocity)
{
	m_Physics.LinearVelocity += addVelocity;
}

void PlanetaryObject::OnConfigRender()
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
