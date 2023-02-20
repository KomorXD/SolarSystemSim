#include "Planet.hpp"

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
