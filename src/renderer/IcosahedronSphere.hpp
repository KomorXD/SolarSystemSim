#pragma once

#include <glm/glm.hpp>
#include <vector>

struct IcosahedronVertex
{
	glm::vec3 Position;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

struct IcosahedronSphereData
{
	std::vector<IcosahedronVertex> Vertices;
	std::vector<uint32_t> Indices;
};

IcosahedronSphereData GenerateIcosahedronSphere(int32_t depth = 3);