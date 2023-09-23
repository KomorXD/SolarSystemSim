#include "Simulator.hpp"
#include "Logger.hpp"

#include <execution>
#include <algorithm>

void SimPhysics::ProgressAllOneStep(std::vector<PlanetaryObject>& planets)
{
	for (auto& planet : planets)
	{
		std::for_each(std::execution::par, planets.begin(), planets.end(), [&](PlanetaryObject& other)
		{
			if (&other == &planet)
			{
				return;
			}

			float distanceSquared = glm::distance2(planet.GetPosition(), other.GetPosition());
			glm::vec3 dir = glm::normalize(other.GetPosition() - planet.GetPosition());
			glm::vec3 addAccel = dir * G_CONSTANT * G_CONSTANT_MULTIPLIER * planet.GetMass() * other.GetMass() / distanceSquared;

			planet.AddAcceleration(addAccel / planet.GetMass());
		});
	}
}

std::vector<glm::vec3> SimPhysics::ApproximateNextNPoints(std::vector<PlanetaryObject>& planets, PlanetaryObject* target, uint32_t N)
{
	std::vector<glm::vec3> points;
	std::vector<PlanetaryObject> initialCopy = planets;
	
	points.reserve(N);
	points.emplace_back(target->GetPosition());

	for (uint32_t i = 0; i < N * 10; i++)
	{
		ProgressAllOneStep(planets);

		for (auto& planet : planets)
		{
			planet.OnUpdate(1.0f / 60.0f);
		}

		if (i % 10)
		{
			points.emplace_back(target->GetPosition());
		}
	}

	planets = initialCopy;

	return points;
}
