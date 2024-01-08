#include <gtest/gtest.h>
#include <filesystem>

#include "../src/Simulator.hpp"
#include "../src/random_utils/SceneSerializer.hpp"
#include "../src/scenes/EditorScene.hpp"
#include "../src/Application.hpp"

#pragma region SimulationTests
TEST(Simulation, SinglePlanet)
{
	std::vector<std::unique_ptr<Planet>> planets;
	planets.push_back(std::make_unique<Planet>());

	Planet& planet = *planets.back();
	planet.GetPhysics().Mass = 1.0f;
	planet.GetPhysics().LinearVelocity = { 0.0f, 0.0f, 0.0f };

	SimPhysics::ProgressAllOneStep(planets);

	ASSERT_EQ(planet.GetPhysics().LinearVelocity, glm::vec3(0.0f)) << "Planet got force while its the only planet";
}

TEST(Simulation, NegativeMass)
{
	std::vector<std::unique_ptr<Planet>> planets;
	planets.push_back(std::make_unique<Planet>());
	planets.push_back(std::make_unique<Planet>());

	Planet& planet1 = *planets[0];
	planet1.GetPhysics().Mass = -1.0f;
	planet1.GetPhysics().LinearVelocity = { 0.0f, 0.0f, 0.0f };

	Planet& planet2 = *planets[1];
	planet2.GetPhysics().Mass = 0.0f;
	planet2.GetPhysics().LinearVelocity = { 0.0f, 0.0f, 0.0f };

	SimPhysics::ProgressAllOneStep(planets);

	ASSERT_EQ(planet1.GetPhysics().LinearVelocity, glm::vec3(0.0f)) << "Negative mass was actually calcualted";
	ASSERT_EQ(planet2.GetPhysics().LinearVelocity, glm::vec3(0.0f)) << "Negative mass was actually calcualted";
}

TEST(Simulation, ZeroMass)
{
	std::vector<std::unique_ptr<Planet>> planets;
	planets.push_back(std::make_unique<Planet>());
	planets.push_back(std::make_unique<Planet>());

	Planet& planet1 = *planets[0];
	planet1.GetPhysics().Mass = 1.0f;
	planet1.GetPhysics().LinearVelocity = { 0.0f, 0.0f, 0.0f };

	Planet& planet2 = *planets[1];
	planet2.GetPhysics().Mass = 0.0f;
	planet2.GetPhysics().LinearVelocity = { 0.0f, 0.0f, 0.0f };

	SimPhysics::ProgressAllOneStep(planets);

	ASSERT_EQ(planet1.GetPhysics().LinearVelocity, glm::vec3(0.0f)) << "Zero mass was actually calculated";
	ASSERT_EQ(planet2.GetPhysics().LinearVelocity, glm::vec3(0.0f)) << "Zero mass was actually calculated";
}

TEST(Simulation, ZeroDistance)
{
	std::vector<std::unique_ptr<Planet>> planets;
	planets.push_back(std::make_unique<Planet>());
	planets.push_back(std::make_unique<Planet>());

	Planet& planet1 = *planets[0];
	planet1.GetTransform().Position = glm::vec3(0.0f);
	planet1.GetPhysics().Mass = 1.0f;
	planet1.GetPhysics().LinearVelocity = { 0.0f, 0.0f, 0.0f };

	Planet& planet2 = *planets[1];
	planet2.GetTransform().Position = glm::vec3(0.0f);
	planet2.GetPhysics().Mass = 0.0000015f;
	planet2.GetPhysics().LinearVelocity = { 0.0f, 0.0f, 0.0f };

	SimPhysics::ProgressAllOneStep(planets);
	
	ASSERT_EQ(planet1.GetPhysics().LinearVelocity, glm::vec3(0.0f)) << "Force was calculated for object with a distance of 0 => div by 0";
	ASSERT_EQ(planet2.GetPhysics().LinearVelocity, glm::vec3(0.0f)) << "Force was calculated for object with a distance of 0 => div by 0";
}

TEST(Simulation, ActuallyCalculating)
{
	std::vector<std::unique_ptr<Planet>> planets;
	planets.push_back(std::make_unique<Planet>());
	planets.push_back(std::make_unique<Planet>());

	Planet& planet1 = *planets[0];
	planet1.GetTransform().Position = glm::vec3(1.0f);
	planet1.GetPhysics().Mass = 1.0f;
	planet1.GetPhysics().LinearVelocity = { 0.0f, 0.0f, 0.0f };

	Planet& planet2 = *planets[1];
	planet2.GetTransform().Position = glm::vec3(0.0f);
	planet2.GetPhysics().Mass = 0.0000015f;
	planet2.GetPhysics().LinearVelocity = { 0.0f, 0.0f, 0.0f };

	SimPhysics::ProgressAllOneStep(planets);

	ASSERT_NE(planet1.GetPhysics().LinearVelocity, glm::vec3(0.0f)) << "No acceleration was added, event though it should have been";
	ASSERT_NE(planet2.GetPhysics().LinearVelocity, glm::vec3(0.0f)) << "No acceleration was added, event though it should have been";
}

TEST(Simulation, EqualForceForSamePlanets)
{
	std::vector<std::unique_ptr<Planet>> planets;
	planets.push_back(std::make_unique<Planet>());
	planets.push_back(std::make_unique<Planet>());

	Planet& planet1 = *planets[0];
	planet1.GetTransform().Position = glm::vec3(1.0f);
	planet1.GetPhysics().Mass = 1.0f;
	planet1.GetPhysics().LinearVelocity = { 0.0f, 0.0f, 0.0f };

	Planet& planet2 = *planets[1];
	planet2.GetTransform().Position = glm::vec3(0.0f);
	planet2.GetPhysics().Mass = 1.0f;
	planet2.GetPhysics().LinearVelocity = { 0.0f, 0.0f, 0.0f };

	SimPhysics::ProgressAllOneStep(planets);

	ASSERT_EQ(planet1.GetPhysics().LinearVelocity, -planet2.GetPhysics().LinearVelocity) << "Forces were not exactly the opposite!";
}
#pragma endregion

#pragma region SceneSerializerTests
TEST(SceneSerializer, SavingScene)
{
	Application app;

	EditorScene scene;
	std::vector<std::unique_ptr<Planet>>& planets = scene.GetPlanetsRef();
	planets.push_back(std::make_unique<Planet>());

	SceneSerializer::SaveScene(scene);
	std::filesystem::path writePath = std::filesystem::current_path().append("Scenes").append("New scene.sscene");
	ASSERT_TRUE(std::filesystem::exists(writePath)) << "Scene " << writePath << " was not saved";
}

TEST(SceneSerializer, LoadingScene)
{
	Application app;

	EditorScene scene;
	std::vector<std::unique_ptr<Planet>>& planets = scene.GetPlanetsRef();
	planets.push_back(std::make_unique<Planet>());
	planets.push_back(std::make_unique<Sun>());
	planets.push_back(std::make_unique<Planet>());

	planets[0]->GetTransform().Position = glm::vec3(1.0f);
	planets[1]->GetTransform().Position = glm::vec3(2.0f);
	planets[2]->GetTransform().Position = glm::vec3(3.0f);

	planets[0]->GetPhysics().Mass = 10.0f;
	planets[1]->GetPhysics().Mass = 20.0f;
	planets[2]->GetPhysics().Mass = 30.0f;

	planets[0]->GetMaterial().Color = glm::vec4(100.0f);
	planets[1]->GetMaterial().Color = glm::vec4(200.0f);
	planets[2]->GetMaterial().Color = glm::vec4(300.0f);

	SceneSerializer::SaveScene(scene);
	std::filesystem::path writePath = std::filesystem::current_path().append("Scenes").append("New scene.sscene");
	std::optional<EditorScene> loadedScene = SceneSerializer::LoadScene(writePath.string());

	ASSERT_TRUE(loadedScene.has_value()) << "Scene was not loaded from filesystem.";

}
#pragma endregion