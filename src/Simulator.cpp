#include "Simulator.hpp"
#include "Logger.hpp"
#include "Application.hpp"

#include <execution>
#include <algorithm>
#include <glm/gtx/norm.hpp>

void SimPhysics::ProgressAllOneStep(std::vector<Planet>& planets, std::vector<SunPlanet>& suns)
{
	for (auto& planet : planets)
	{
			std::for_each(std::execution::par, planets.begin(), planets.end(), [&](Planet& other)
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

		std::for_each(std::execution::par, suns.begin(), suns.end(), [&](SunPlanet& other)
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

	for (auto& sun : suns)
	{				  
		std::for_each(std::execution::par, planets.begin(), planets.end(), [&](Planet& other)
			{		  
				if (&other == &sun)
				{	  
					return;
				}	  
					  
				float distanceSquared = glm::distance2(sun.GetTransform().Position, other.GetTransform().Position);
				glm::vec3 dir = glm::normalize(other.GetTransform().Position - sun.GetTransform().Position);
				glm::vec3 addAccel = dir * G_CONSTANT * G_CONSTANT_MULTIPLIER * sun.GetPhysics().Mass * other.GetPhysics().Mass / distanceSquared;
					  
				sun.AddVelocity(Application::TPS_STEP * addAccel / sun.GetPhysics().Mass);
			});		  
					  
		std::for_each(std::execution::par, suns.begin(), suns.end(), [&](SunPlanet& other)
			{		  
				if (&other == &sun)
				{	  
					return;
				}	  
					  
				float distanceSquared = glm::distance2(sun.GetTransform().Position, other.GetTransform().Position);
				glm::vec3 dir = glm::normalize(other.GetTransform().Position - sun.GetTransform().Position);
				glm::vec3 addAccel = dir * G_CONSTANT * G_CONSTANT_MULTIPLIER * sun.GetPhysics().Mass * other.GetPhysics().Mass / distanceSquared;
					  
				sun.AddVelocity(Application::TPS_STEP * addAccel / sun.GetPhysics().Mass);
			});		  
	}				  
}					  

std::vector<glm::vec3> SimPhysics::ApproximateNextNPoints(std::vector<Planet>& planets, std::vector<SunPlanet>& suns, Planet* target, uint32_t N)
{
	std::vector<glm::vec3> points;
	std::vector<Planet> planetsCopy = planets;
	std::vector<SunPlanet> sunsCopy = suns;
	Planet* targetCopy = nullptr;
	
	for (size_t i = 0; i < planets.size(); i++)
	{
		if (&planets[i] == target)
		{
			targetCopy = &planetsCopy[i];
			break;
		}
	}

	if (targetCopy == nullptr)
	{
		for (size_t i = 0; i < suns.size(); i++)
		{
			if (&suns[i] == target)
			{
				targetCopy = &sunsCopy[i];
				break;
			}
		}
	}
	
	points.reserve(N);
	points.emplace_back(targetCopy->GetTransform().Position);

	for (uint32_t i = 0; i < N * 10; i++)
	{
		ProgressAllOneStep(planetsCopy, sunsCopy);

		for (auto& planet : planetsCopy)
		{
			planet.OnUpdate(Application::TPS_STEP);
		}

		for (auto& sun : sunsCopy)
		{
			sun.OnUpdate(Application::TPS_STEP);
		}

		if (i % 2 == 0)
		{
			points.emplace_back(targetCopy->GetTransform().Position);
		}
	}

	return points;
}

std::vector<glm::vec3> SimPhysics::ApproximateRelativeNextNPoints(std::vector<Planet>& planets, std::vector<SunPlanet>& suns, Planet* target, uint32_t N)
{
	Planet* parent = target->GetRelative();

	if (parent == nullptr)
	{
		return {};
	}

	std::vector<glm::vec3> relToPlanetVectors;
	std::vector<Planet> planetsCopy = planets;
	std::vector<SunPlanet> sunsCopy = suns;

	Planet* targetCopy = nullptr;
	Planet* targetRelCopy = nullptr;

	for (size_t i = 0; i < planets.size(); i++)
	{
		if (&planets[i] == target)
		{
			targetCopy = &planetsCopy[i];
		}
		else if (&planets[i] == parent)
		{
			targetRelCopy = &planetsCopy[i];
		}
	}
	
	if (targetCopy == nullptr || targetRelCopy == nullptr)
	{
		for (size_t i = 0; i < suns.size(); i++)
		{
			if (&suns[i] == target)
			{
				targetCopy = &sunsCopy[i];
			}
			else if (&suns[i] == parent)
			{
				targetRelCopy = &sunsCopy[i];
			}
		}
	}
	
	relToPlanetVectors.reserve(N);
	relToPlanetVectors.emplace_back(targetCopy->GetTransform().Position - targetRelCopy->GetTransform().Position);
			 
	for (uint32_t i = 0; i < N * 10; i++)
	{
		ProgressAllOneStep(planetsCopy, sunsCopy);

		for (auto& planet : planetsCopy)
		{
			planet.OnUpdate(Application::TPS_STEP);
		}

		for (auto& sun : sunsCopy)
		{
			sun.OnUpdate(Application::TPS_STEP);
		}

		if (i % 2 == 0)
		{
			relToPlanetVectors.emplace_back(targetCopy->GetTransform().Position - targetRelCopy->GetTransform().Position);
		}
	}
			 
	return relToPlanetVectors;
}
