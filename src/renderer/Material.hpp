#pragma once

#include <glm/glm.hpp>

struct Material
{
	glm::vec4 Ambient  = { 0.44f, 0.44f, 0.44f, 1.0f };
	glm::vec4 Diffuse  = { 0.44f, 0.44f, 0.44f, 1.0f };
	glm::vec4 Specular = { 0.44f, 0.44f, 0.44f, 1.0f };
	float Shininess = 1.0f;
};