#include "SceneSerializer.hpp"
#include "../Logger.hpp"
#include "../TextureManager.hpp"
#include "../scenes/EditorScene.hpp"

#include <filesystem>
#include <fstream>

EditorScene SceneSerializer::LoadScene(const std::string& path)
{
	return EditorScene{};
}

bool SceneSerializer::SaveScene(const EditorScene& scene)
{
	std::fstream file;
	std::filesystem::path writePath = std::filesystem::current_path().append("Scenes");

	if (!std::filesystem::exists(writePath))
	{
		std::filesystem::create_directory(writePath);
	}

	writePath.append(scene.m_SceneName + ".sscene");

	file.open(writePath, std::ios::out | std::ios::binary);
	if (!file.good())
	{
		LOG_ERROR("Saving scene {} failed: file {} could not be created/opened.", scene.m_SceneName, std::filesystem::absolute(writePath).string());

		file.close();
		return false;
	}

	// Header
	file.write("SSSSCENE", 9);

	// Scene info
	int32_t sceneNameLen = scene.m_SceneName.length();
	file.write((char*)&sceneNameLen, sizeof(int32_t));
	file.write(scene.m_SceneName.data(), scene.m_SceneName.length());

	// Camera
	file.write((char*)&scene.m_Camera.m_AspectRatio, sizeof(float));
	file.write((char*)&scene.m_Camera.m_NearClip,	 sizeof(float));
	file.write((char*)&scene.m_Camera.m_FarClip,	 sizeof(float));
	file.write((char*)&scene.m_Camera.m_FOV,		 sizeof(float));
	file.write((char*)&scene.m_Camera.m_Pitch,		 sizeof(float));
	file.write((char*)&scene.m_Camera.m_Yaw,		 sizeof(float));
	file.write((char*)&scene.m_Camera.m_Position,	 sizeof(glm::vec3));
	
	// Objects
	int32_t objectsCount = scene.m_Planets.size();
	file.write((char*)&objectsCount, sizeof(int32_t));

	for (auto& planet : scene.m_Planets)
	{
		Transform& transform = planet->m_Transform;
		Physics& physics	 = planet->m_Physics;
		Material& material   = planet->m_Material;

		// Object info
		int32_t tagLen = planet->m_Tag.length();
		file.write((char*)&tagLen, sizeof(int32_t));
		file.write(planet->m_Tag.c_str(), tagLen);
		file.write((char*)&planet->m_Type, sizeof(ObjectType));

		// Transform
		file.write((char*)&transform.Position, sizeof(glm::vec3));
		file.write((char*)&transform.Rotation, sizeof(glm::vec3));
		file.write((char*)&transform.Scale,	   sizeof(glm::vec3));

		// Physics
		file.write((char*)&physics.LinearVelocity,	sizeof(glm::vec3));
		file.write((char*)&physics.AngularVelocity, sizeof(glm::vec3));
		file.write((char*)&physics.Mass,			sizeof(float));

		// Material
		file.write((char*)&material.Color,	   sizeof(glm::vec4));
		file.write((char*)&material.Shininess, sizeof(float));
		file.write((char*)&material.Roughness, sizeof(float));
		
		// Textures
		std::string albedoPath = TextureManager::GetTexture(material.TextureID)
			.value_or(TextureManager::GetTexture(TextureManager::DEFAULT_ALBEDO).value()).Path;
		int32_t albedoPathLen = albedoPath.length();
		file.write((char*)&albedoPathLen, sizeof(int32_t));
		file.write(albedoPath.c_str(), albedoPathLen);

		std::string normalPath = TextureManager::GetTexture(material.NormalMapTextureID)
			.value_or(TextureManager::GetTexture(TextureManager::DEFAULT_NORMAL).value()).Path;
		int32_t normalPathLen = normalPath.length();
		file.write((char*)&normalPathLen, sizeof(int32_t));
		file.write(normalPath.c_str(), normalPathLen);

		std::string specularPath = TextureManager::GetTexture(material.SpecularMapTextureID)
			.value_or(TextureManager::GetTexture(TextureManager::DEFAULT_SPECULAR).value()).Path;
		int32_t specularPathLen = specularPath.length();
		file.write((char*)&specularPathLen, sizeof(int32_t));
		file.write(specularPath.c_str(), specularPathLen);

		// If it's a star, point light info
		if (planet->m_Type == ObjectType::Sun)
		{
			Sun* sun = (Sun*)planet.get();

			file.write((char*)&sun->m_Light.Color, sizeof(glm::vec3));
			file.write((char*)&sun->m_Light.Intensity, sizeof(float));
		}
	}

	file.close();
	LOG_INFO("Saved scene {} to: {}", scene.m_SceneName, std::filesystem::absolute(writePath).string());

	return true;
}
