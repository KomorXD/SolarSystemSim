#pragma once

#include <glm/glm.hpp>
#include <vector>

struct IcosahedronSphereData
{
	std::vector<glm::vec3> Vertices;
	std::vector<uint32_t> Indices;
};

IcosahedronSphereData GenerateIcosahedronSphere(int32_t depth = 3);