#pragma once

#include <glm/glm.hpp>

class Planet
{
public:
	Planet(const glm::vec3& position);

	void Move(const glm::vec3& offset);
	void SetPosition(const glm::vec3& position);
	void SetColor(const glm::vec4& color);

	inline glm::vec3 GetPosition() const { return m_Position; }
	inline glm::vec4 GetColor()	   const { return m_Color;    }
	inline float GetRadius()	   const { return m_Radius;   }
	inline uint32_t GetEntityID()  const { return m_EntityID; }

	inline static constexpr uint32_t MAX_PLANETS = 254;

private:
	glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
	glm::vec4 m_Color = { 0.2f, 0.2f, 0.2f, 1.0f };

	float m_Radius = 1.0f;

	uint32_t m_EntityID = 0;
};