#pragma once

#include "SceneObject.hpp"

#include <memory>

class Planet : public SceneObject
{
public:
	Planet();
	virtual ~Planet() = default;

	virtual void OnUpdate(float ts) override;
	virtual void OnTick()			override;
	virtual void OnConfigRender()	override;

	virtual std::unique_ptr<Planet> Clone();

	inline Physics&  GetPhysics()  { return m_Physics;  }
	inline Material& GetMaterial() { return m_Material; }

	inline float GetMinRadius() const { return glm::min(glm::min(m_Transform.Scale.x, m_Transform.Scale.y), m_Transform.Scale.z); }
	inline float GetMaxRadius() const { return glm::max(glm::max(m_Transform.Scale.x, m_Transform.Scale.y), m_Transform.Scale.z); }

	void SetRelativePlanet(Planet* planet);
	inline Planet* GetRelativePlanet() const { return m_RelativePathPlanet; }

	// For development
	void SetMass(float mass) { m_Physics.Mass = mass; }
	void SetRadius(float radius) { m_Transform.Scale = glm::vec3(radius); }
	//

protected:
	Physics  m_Physics;
	Material m_Material;

	Planet* m_RelativePathPlanet = nullptr;
};
