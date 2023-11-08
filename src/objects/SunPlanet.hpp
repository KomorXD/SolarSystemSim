#pragma once

#include "Planet.hpp"

class SunPlanet : public Planet
{
public:
	SunPlanet(const glm::vec3& position);

	void OnConfigRender() override;

	inline PointLight GetPointLight() const { return m_Light; }
	
private:
	PointLight m_Light;
};