#include <gtest/gtest.h>

#include "../src/Simulator.hpp"

TEST(Testowy, LolXd)
{
	int a = 7 + 2;

	EXPECT_EQ(a, 9);
}

TEST(Testowy, LolXd2)
{
	int a = 7 + 2;

	EXPECT_EQ(a, 11);
}

TEST(BlagamXd, lol)
{
	Planet planet;

	planet.GetTransform().Position.x += 5.0f;

	EXPECT_EQ(planet.GetTransform().Position, glm::vec3(5.0f, 0.0f, 0.0f));
}