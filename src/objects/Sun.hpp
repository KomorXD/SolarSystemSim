#pragma once

#include "Planet.hpp"

class Sun : public Planet
{
public:
	Sun();
	virtual ~Sun() = default;

	virtual void OnConfigRender() override;
	virtual std::unique_ptr<Planet> Clone() override;

	inline PointLight& GetLight() { return m_Light; }

private:
	PointLight m_Light;

	friend class SceneSerializer;
};