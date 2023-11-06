#pragma once

#include <glm/glm.hpp>

struct Material
{
	glm::vec4 Color = { 0.44f, 0.44f, 0.44f, 1.0f };
	float Shininess = 1.0f;
	int32_t TextureID = 1;
};