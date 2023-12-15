#pragma once

#include "objects/Planet.hpp"

#include <vector>

class SimPhysics
{
public:
	static void ProgressAllOneStep(std::vector<std::unique_ptr<Planet>>& planets);
	static std::vector<glm::vec3> ApproximateNextNPoints(std::vector<std::unique_ptr<Planet>>& planets, Planet* target, uint32_t N);
	static std::vector<glm::vec3> ApproximateRelativeNextNPoints(std::vector<std::unique_ptr<Planet>>& planets, Planet* target, uint32_t N);

	static inline constexpr double G_CONSTANT = 6.674e-11;
	static inline float G_CONSTANT_MULTIPLIER = 1.0f;

	// 1 mass unit = sun's mass [kg]
	static inline constexpr double SUN_MASS = 1.989e30;

	// 10 distance units = sun to earth distance [meters]
	static inline constexpr double SUN_TO_EARTH_DIST = 1.496e10;

	// Factor to scale forces by
	static inline constexpr double SCALE_FACTOR = (G_CONSTANT * SUN_MASS * SUN_MASS) / (SUN_TO_EARTH_DIST * SUN_TO_EARTH_DIST);
};