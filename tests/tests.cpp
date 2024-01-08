#include <gtest/gtest.h>
#include <filesystem>

#include "../src/Simulator.hpp"
#include "../src/random_utils/SceneSerializer.hpp"
#include "../src/scenes/EditorScene.hpp"
#include "../src/Application.hpp"
#include "../src/renderer/IcosahedronSphere.hpp"


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
#pragma endregion

#pragma region SphereGenerationTests
TEST(SphereGeneration, VerticesIndicesDepth1)
{
	IcosahedronSphereData data = GenerateIcosahedronSphere(1);

	ASSERT_EQ(data.Vertices.size(), 240) << "Incorrect amount of vertices for depth 1";
	ASSERT_EQ(data.Indices.size(),  240) << "Incorrect amount of indices for depth 1";

	for (const IcosahedronVertex& vertex : data.Vertices)
	{
		ASSERT_FLOAT_EQ(glm::length(vertex.Position),  1.0f) << "Normal vector is not normalized";
		ASSERT_FLOAT_EQ(glm::length(vertex.Tangent),   1.0f) << "Tangent vector is not normalized";
		ASSERT_FLOAT_EQ(glm::length(vertex.Bitangent), 1.0f) << "Bitangent vector is not normalized";
	}
}

TEST(SphereGeneration, VerticesIndicesDepth2)
{
	IcosahedronSphereData data = GenerateIcosahedronSphere(2);

	ASSERT_EQ(data.Vertices.size(), 960) << "Incorrect amount of vertices for depth 2";
	ASSERT_EQ(data.Indices.size(),  960) << "Incorrect amount of indices for depth 2";

	for (const IcosahedronVertex& vertex : data.Vertices)
	{
		ASSERT_FLOAT_EQ(glm::length(vertex.Position), 1.0f) << "Normal vector is not normalized";
		ASSERT_FLOAT_EQ(glm::length(vertex.Tangent), 1.0f) << "Tangent vector is not normalized";
		ASSERT_FLOAT_EQ(glm::length(vertex.Bitangent), 1.0f) << "Bitangent vector is not normalized";
	}
}

TEST(SphereGeneration, VerticesIndicesDepth3)
{
	IcosahedronSphereData data = GenerateIcosahedronSphere(3);

	ASSERT_EQ(data.Vertices.size(), 3840) << "Incorrect amount of vertices for depth 3";
	ASSERT_EQ(data.Indices.size(),  3840) << "Incorrect amount of indices for depth 3";

	for (const IcosahedronVertex& vertex : data.Vertices)
	{
		ASSERT_FLOAT_EQ(glm::length(vertex.Position), 1.0f) << "Normal vector is not normalized";
		ASSERT_FLOAT_EQ(glm::length(vertex.Tangent), 1.0f) << "Tangent vector is not normalized";
		ASSERT_FLOAT_EQ(glm::length(vertex.Bitangent), 1.0f) << "Bitangent vector is not normalized";
	}
}

TEST(SphereGeneration, VerticesIndicesDepth4)
{
	IcosahedronSphereData data = GenerateIcosahedronSphere(4);

	ASSERT_EQ(data.Vertices.size(), 15360) << "Incorrect amount of vertices for depth 4";
	ASSERT_EQ(data.Indices.size(),  15360) << "Incorrect amount of indices for depth 4";

	for (const IcosahedronVertex& vertex : data.Vertices)
	{
		ASSERT_FLOAT_EQ(glm::length(vertex.Position), 1.0f) << "Normal vector is not normalized";
		ASSERT_FLOAT_EQ(glm::length(vertex.Tangent), 1.0f) << "Tangent vector is not normalized";
		ASSERT_FLOAT_EQ(glm::length(vertex.Bitangent), 1.0f) << "Bitangent vector is not normalized";
	}
}
#pragma endregion