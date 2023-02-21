#include "Random.hpp"

thread_local std::mt19937 Random::s_Engine(std::default_random_engine{});

int32_t Random::IntInRange(int32_t low, int32_t high)
{
	std::uniform_int_distribution<int32_t> dist(low, high);

	return dist(s_Engine);
}

float Random::FloatInRange(float low, float high)
{
	std::uniform_real_distribution<float> dist(low, high);

	return dist(s_Engine);
}
