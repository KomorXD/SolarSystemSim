#include "Simulator.hpp"
#include "Logger.hpp"
#include "Application.hpp"

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

			planet.AddVelocity(Application::TPS_STEP * addAccel / planet.GetMass());
		});
	}
}

std::vector<glm::vec3> SimPhysics::ApproximateNextNPoints(std::vector<PlanetaryObject>& planets, PlanetaryObject* target, uint32_t N)
{
	std::vector<glm::vec3> points;
	std::vector<PlanetaryObject> planetsCopy = planets;
	
	size_t copiedPlanetIdx{};
	for (size_t i = 0; i < planets.size(); i++)
	{
		if (&planets[i] == target)
		{
			copiedPlanetIdx = i;
			break;
		}
	}

	PlanetaryObject* targetCopy = &planetsCopy[copiedPlanetIdx];
	
	points.reserve(N);
	points.emplace_back(targetCopy->GetPosition());

	for (uint32_t i = 0; i < N * 10; i++)
	{
		ProgressAllOneStep(planetsCopy);

		for (auto& planet : planetsCopy)
		{
			planet.OnUpdate(Application::TPS_STEP);
		}

		if (i % 10)
		{
			points.emplace_back(targetCopy->GetPosition());
		}
	}

	return points;
}
