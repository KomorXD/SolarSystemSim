#include "Simulator.hpp"
#include "Logger.hpp"
#include "Application.hpp"

#include <execution>
#include <algorithm>
#include <glm/gtx/norm.hpp>

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

			float distanceSquared = glm::distance2(planet.GetTransform().Position, other.GetTransform().Position);
			glm::vec3 dir = glm::normalize(other.GetTransform().Position - planet.GetTransform().Position);
			glm::vec3 addAccel = dir * G_CONSTANT * G_CONSTANT_MULTIPLIER * planet.GetPhysics().Mass * other.GetPhysics().Mass / distanceSquared;

			planet.AddVelocity(Application::TPS_STEP * addAccel / planet.GetPhysics().Mass);
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
	points.emplace_back(targetCopy->GetTransform().Position);

	for (uint32_t i = 0; i < N * 10; i++)
	{
		ProgressAllOneStep(planetsCopy);

		for (auto& planet : planetsCopy)
		{
			planet.OnUpdate(Application::TPS_STEP);
		}

		if (i % 10)
		{
			points.emplace_back(targetCopy->GetTransform().Position);
		}
	}

	return points;
}

std::vector<glm::vec3> SimPhysics::ApproximateRelativeNextNPoints(std::vector<PlanetaryObject>& planets, PlanetaryObject* target, uint32_t N)
{
	PlanetaryObject* parent = target->GetRelative();

	if (parent == nullptr)
	{
		return {};
	}

	std::vector<glm::vec3> relToPlanetVectors;
	std::vector<PlanetaryObject> planetsCopy = planets;
			 
	size_t copiedPlanetIdx{};
	size_t copiedParentIdx{};
	for (size_t i = 0; i < planets.size(); i++)
	{		 
		if (&planets[i] == target)
		{
			copiedPlanetIdx = i;
		}
		else if (&planets[i] == parent)
		{
			copiedParentIdx = i;
		}
	}		 
			 
	PlanetaryObject* targetCopy = &planetsCopy[copiedPlanetIdx];
	PlanetaryObject* targetRelCopy = &planetsCopy[copiedParentIdx];
			 
	relToPlanetVectors.reserve(N);
	relToPlanetVectors.emplace_back(targetCopy->GetTransform().Position - targetRelCopy->GetTransform().Position);
			 
	for (uint32_t i = 0; i < N * 10; i++)
	{
		ProgressAllOneStep(planetsCopy);

		for (auto& planet : planetsCopy)
		{
			planet.OnUpdate(Application::TPS_STEP);
		}

		if (i % 10)
		{
			relToPlanetVectors.emplace_back(targetCopy->GetTransform().Position - targetRelCopy->GetTransform().Position);
		}
	}
			 
	return relToPlanetVectors;
}
