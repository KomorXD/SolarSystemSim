#pragma once

#include "objects/Planet.hpp"

#include <vector>

class SimPhysics
{
public:
	static void ProgressAllOneStep(std::vector<PlanetaryObject>& planets);
	static std::vector<glm::vec3> ApproximateNextNPoints(std::vector<PlanetaryObject>& planets, PlanetaryObject* target, uint32_t N);
	static std::vector<glm::vec3> ApproximateRelativeNextNPoints(std::vector<PlanetaryObject>& planets, PlanetaryObject* target, uint32_t N);

	static inline constexpr float G_CONSTANT = 6.6743e-2;
	static inline constexpr float G_CONSTANT_MULTIPLIER = 1.0f;
};