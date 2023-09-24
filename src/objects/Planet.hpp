#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

class PlanetaryObject
{
public:
	PlanetaryObject(const glm::vec3& position);

	void OnUpdate(float ts);

	void Move(const glm::vec3& offset);
	
	void SetPosition(const glm::vec3& position);
	void SetScale(const glm::vec3& scale);
	void SetRadius(float radius);
	void SetRotation(const glm::vec3& rotation);
	void SetColor(const glm::vec4& color);
	void SetMass(float mass);

	void SetVelocity(const glm::vec3& velocity);
	void AddVelocity(const glm::vec3& addVelocity);

	void OnConfigRender();

	inline glm::vec3 GetPosition() const { return m_Position; }
	inline glm::vec3 GetRotation() const { return m_Rotation; }
	inline glm::vec3 GetScale()	   const { return m_Scale;	  }
	inline glm::vec4 GetColor()	   const { return m_Color;    }
	inline float GetRadius()	   const { return m_Radius;   }
	inline float GetMass()		   const { return m_Mass;	  }
	inline uint32_t GetEntityID()  const { return m_EntityID; }

	inline glm::mat4 GetTransform() const
	{
		return glm::translate(glm::mat4(1.0f), m_Position) 
			* glm::scale(glm::mat4(1.0f), m_Scale)
			* glm::toMat4(glm::quat(m_Rotation));
	}

	inline static constexpr uint32_t MAX_PLANETS = 254;

private:
	glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_Scale	 = { 1.0f, 1.0f, 1.0f };
	glm::vec4 m_Color	 = { 0.44f, 0.44f, 0.44f, 1.0f };

	glm::vec3 m_Velocity	 = { 0.0f, 0.0f, 0.0f };

	float m_Radius = 1.0f;
	float m_Mass   = 1.0f;

	uint32_t m_EntityID = 0;
};