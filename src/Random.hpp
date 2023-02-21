#pragma once

#include <random>

class Random
{
public:
	static int32_t IntInRange(int32_t low, int32_t high);
	static float FloatInRange(float low, float high);

private:
	static thread_local std::mt19937 s_Engine;
};