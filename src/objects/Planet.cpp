#include "Planet.hpp"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

static uint32_t s_IdCounter = 1;

PlanetaryObject::PlanetaryObject(const glm::vec3& position)
	: m_Position(position)
{
	m_EntityID = s_IdCounter;
	++s_IdCounter;
}

void PlanetaryObject::Move(const glm::vec3& offset)
{
	m_Position += offset;
}

void PlanetaryObject::SetPosition(const glm::vec3& position)
{
	m_Position = position;
}

void PlanetaryObject::SetScale(const glm::vec3& scale)
{
	m_Scale = scale;
	m_Radius = glm::max(glm::max(scale.x, scale.y), scale.z);
}

void PlanetaryObject::SetRadius(float radius)
{
	m_Radius = radius;
	m_Scale = glm::vec3(radius);
}

void PlanetaryObject::SetRotation(const glm::vec3& rotation)
{
	m_Rotation = rotation;
}

void PlanetaryObject::SetColor(const glm::vec4& color)
{
	m_Color = color;
}

void PlanetaryObject::SetMass(float mass)
{
	m_Mass = mass;
}

void PlanetaryObject::SetVelocity(const glm::vec3& velocity)
{
	m_Velocity = velocity;
}

void PlanetaryObject::SetAcceleration(const glm::vec3& acceleration)
{
	m_Acceleration = acceleration;
}

void PlanetaryObject::OnConfigRender()
{
	ImGui::BeginChild("Sphere settings");
	ImGui::Text("Planet #%d", m_EntityID);
	ImGui::DragFloat3("Position", glm::value_ptr(m_Position), 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
	ImGui::DragFloat3("Rotation", glm::value_ptr(m_Rotation), 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
	ImGui::DragFloat3("Scale", glm::value_ptr(m_Scale), 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
	ImGui::ColorEdit4("Color", glm::value_ptr(m_Color));
	ImGui::DragFloat3("Velocity", glm::value_ptr(m_Velocity), 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
	ImGui::DragFloat3("Acceleration", glm::value_ptr(m_Acceleration), 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
	ImGui::DragFloat("Mass", &m_Mass, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
	ImGui::EndChild();
}
