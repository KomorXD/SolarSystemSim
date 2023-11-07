#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

#include "../objects/Components.hpp"

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
	void SetMass(float mass);
	void SetTextureID(int32_t id);

	void SetVelocity(const glm::vec3& velocity);
	void AddVelocity(const glm::vec3& addVelocity);

	void OnConfigRender();

	inline std::string GetTag()     const { return m_Tag;		}
	inline Transform GetTransform() const { return m_Transform; }
	inline Physics GetPhysics()     const { return m_Physics;   }
	inline Material GetMaterial()   const { return m_Material;  }
	inline float GetMinRadius()	    const { return glm::min(glm::min(m_Transform.Scale.x, m_Transform.Scale.y), m_Transform.Scale.z); }
	inline float GetMaxRadius()	    const { return glm::max(glm::max(m_Transform.Scale.x, m_Transform.Scale.y), m_Transform.Scale.z); }
	inline uint32_t GetEntityID()   const { return m_EntityID;  }
	
	inline static constexpr uint32_t MAX_PLANETS = 254;

private:
	std::string m_Tag;
	
	Transform m_Transform;
	Physics   m_Physics;
	Material  m_Material;

	uint32_t m_EntityID = 0;
};