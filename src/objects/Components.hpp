#pragma once

#include <glm/glm.hpp>

struct Transform
{
	glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Scale	   = { 1.0f, 1.0f, 1.0f };

	glm::mat4 Matrix() const;
	void OnImGuiRender();
};

struct Physics
{
	glm::vec3 LinearVelocity  = { 0.0f, 0.0f, 0.0f };
	glm::vec3 AngularVelocity = { 0.0f, 0.0f, 0.0f };
	float Mass = 1.0f;

	void OnImGuiRender();
};

struct Material
{
	glm::vec4 Color = { 0.44f, 0.44f, 0.44f, 1.0f };
	float Shininess = 1.0f;
	int32_t TextureID = 1;
	bool TextureInUse = true;

	void OnImGuiRender();
};

struct PointLight
{
	glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
	float Intensity = 1.0f;

	void OnImGuiRender();
};