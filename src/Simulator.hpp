#pragma once

#include "objects/Planet.hpp"

#include <vector>

class SimPhysics
{
public:
	static void ProgressAllOneStep(std::vector<std::unique_ptr<Planet>>& planets);
	static std::vector<glm::vec3> ApproximateNextNPoints(std::vector<std::unique_ptr<Planet>>& planets, Planet* target, uint32_t N);
	static std::vector<glm::vec3> ApproximateRelativeNextNPoints(std::vector<std::unique_ptr<Planet>>& planets, Planet* target, uint32_t N);

	static inline constexpr float G_CONSTANT = 6.6743e-2f;
	static inline constexpr float G_CONSTANT_MULTIPLIER = 1.0f;
};