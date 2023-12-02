#include "Simulator.hpp"
#include "Logger.hpp"
#include "Application.hpp"

#include <execution>
#include <algorithm>
#include <glm/gtx/norm.hpp>

void SimPhysics::ProgressAllOneStep(std::vector<std::unique_ptr<Planet>>& planets)
{
	for (auto& planet : planets)
	{
			std::for_each(std::execution::par, planets.begin(), planets.end(), [&](std::unique_ptr<Planet>& other)
			{
				if (other == planet)
				{
					return;
				}

				float distanceSquared = glm::distance2(planet->GetTransform().Position, other->GetTransform().Position);
				glm::vec3 dir = glm::normalize(other->GetTransform().Position - planet->GetTransform().Position);
				glm::vec3 addAccel = dir * G_CONSTANT * G_CONSTANT_MULTIPLIER * planet->GetPhysics().Mass * other->GetPhysics().Mass / distanceSquared;

				glm::vec3 linVel = planet->GetPhysics().LinearVelocity;
				planet->GetPhysics().LinearVelocity = linVel + (Application::TPS_STEP * Application::TPS_MULTIPLIER * addAccel / planet->GetPhysics().Mass);
			});
	}				  
}					  

std::vector<glm::vec3> SimPhysics::ApproximateNextNPoints(std::vector<std::unique_ptr<Planet>>& planets, Planet* target, uint32_t N)
{
	std::vector<glm::vec3> points;
	std::vector<std::unique_ptr<Planet>> planetsCopy;

	planetsCopy.reserve(planets.size());

	for (auto& planet : planets)
	{
		planetsCopy.push_back(planet->Clone());
	}

	SceneObject::CleanLastNIDs(planetsCopy.size());

	Planet* targetCopy = nullptr;
	
	for (size_t i = 0; i < planets.size(); i++)
	{
		if (planets[i].get() == target)
		{
			targetCopy = planetsCopy[i].get();
			break;
		}
	}
	
	points.reserve(N);
	points.emplace_back(targetCopy->GetTransform().Position);

	for (uint32_t i = 0; i < N * 10; i++)
	{
		ProgressAllOneStep(planetsCopy);

		for (auto& planet : planetsCopy)
		{
			planet->OnTick();
		}

		if (i % 2 == 0)
		{
			points.emplace_back(targetCopy->GetTransform().Position);
		}
	}

	return points;
}

std::vector<glm::vec3> SimPhysics::ApproximateRelativeNextNPoints(std::vector<std::unique_ptr<Planet>>& planets, Planet* target, uint32_t N)
{
	Planet* parent = target->GetRelativePlanet();

	if (parent == nullptr)
	{
		return {};
	}

	std::vector<glm::vec3> relToPlanetVectors;
	std::vector<std::unique_ptr<Planet>> planetsCopy;

	planetsCopy.reserve(planets.size());

	for (auto& planet : planets)
	{
		planetsCopy.push_back(planet->Clone());
	}

	SceneObject::CleanLastNIDs(planetsCopy.size());

	Planet* targetCopy = nullptr;
	Planet* targetRelCopy = nullptr;

	for (size_t i = 0; i < planets.size(); i++)
	{
		if (planets[i].get() == target)
		{
			targetCopy = planetsCopy[i].get();
		}
		else if (planets[i].get() == parent)
		{
			targetRelCopy = planetsCopy[i].get();
		}
	}
	
	relToPlanetVectors.reserve(N);
	relToPlanetVectors.emplace_back(targetCopy->GetTransform().Position - targetRelCopy->GetTransform().Position);
			 
	for (uint32_t i = 0; i < N * 10; i += 2)
	{
		ProgressAllOneStep(planetsCopy);

		for (auto& planet : planetsCopy)
		{
			planet->OnTick();
		}

		relToPlanetVectors.emplace_back(targetCopy->GetTransform().Position - targetRelCopy->GetTransform().Position);
	}
			 
	return relToPlanetVectors;
}
