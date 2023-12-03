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

				glm::dvec3 dir = glm::normalize(other->GetTransform().Position - planet->GetTransform().Position);
				double distance2 = glm::distance2(planet->GetTransform().Position, other->GetTransform().Position);
				double mass1 = (double)planet->GetPhysics().Mass;
				double mass2 = (double)other->GetPhysics().Mass;

				glm::dvec3 F = dir * mass1 * mass2 / distance2;
				F *= SCALE_FACTOR;
				glm::dvec3 addAccel = F / (mass1 * SUN_MASS);
				glm::vec3 fAddAccel = addAccel * 3.0;

				glm::vec3 linVel = planet->GetPhysics().LinearVelocity;
				planet->GetPhysics().LinearVelocity = linVel + (Application::TPS_STEP * Application::TPS_MULTIPLIER * fAddAccel);
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
