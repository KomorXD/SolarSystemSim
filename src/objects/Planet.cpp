#include "Planet.hpp"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

static uint32_t s_IdCounter = 1;

Planet::Planet(const glm::vec3& position)
	: m_Position(position)
{
	m_EntityID = s_IdCounter;
	++s_IdCounter;
}

void Planet::Move(const glm::vec3& offset)
{
	m_Position += offset;
}

void Planet::SetPosition(const glm::vec3& position)
{
	m_Position = position;
}

void Planet::SetColor(const glm::vec4& color)
{
	m_Color = color;
}

void Planet::OnConfigRender()
{
	ImGui::BeginChild("Sphere settings");
	ImGui::Text("Planet #%d", m_EntityID);
	ImGui::DragFloat3("Position", glm::value_ptr(m_Position), 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
	ImGui::ColorEdit4("Color", glm::value_ptr(m_Color));
	ImGui::DragFloat("Radius", &m_Radius, 0.01f, 0.0f, 10.0f, "%.2f");
	ImGui::EndChild();
}
