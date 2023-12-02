#pragma once

#include "SceneObject.hpp"

#include <memory>

class Planet : public SceneObject
{
public:
	Planet();
	Planet(const Planet& other);
	virtual ~Planet() = default;

	virtual void OnUpdate(float ts) override;
	virtual void OnTick()			override;
	virtual void OnConfigRender()	override;
	virtual void OnSimDataRender();

	virtual std::unique_ptr<Planet> Clone();

	inline Physics&  GetPhysics()  { return m_Physics;  }
	inline Material& GetMaterial() { return m_Material; }

	inline float GetMinRadius() const { return m_Transform.Scale.x; }
	inline float GetMaxRadius() const { return m_Transform.Scale.x; }

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

	friend class SceneSerializer;
};
