#include "SceneSerializer.hpp"
#include "../Logger.hpp"
#include "../Timer.hpp"
#include "../TextureManager.hpp"
#include "../scenes/EditorScene.hpp"

#include <filesystem>
#include <fstream>

#define READ_AND_ASSERT_EOF(file, dest, type) if(file.eof()) { return {}; } file.read((char*)&dest, sizeof(type))

std::optional<EditorScene> SceneSerializer::LoadScene(const std::string& path)
{
	FUNC_PROFILE();

	std::filesystem::path readPath = std::filesystem::path(path);
	if (!std::filesystem::exists(readPath))
	{
		LOG_ERROR("Path {} does not exist.", readPath.string());

		return {};
	}

	std::fstream file(readPath, std::ios::in | std::ios::binary);
	if (!file.good())
	{
		LOG_ERROR("File {} could not be opened.", std::filesystem::absolute(readPath).string());

		file.close();
		return {};
	}

	char buf[512]{};
	file.read(buf, 9);

	if (strcmp(buf, "SSSSCENE") != 0)
	{
		LOG_ERROR("Wrong scene file header.");

		file.close();
		return {};
	}

	EditorScene scene;

	// Scene info
	int32_t sceneNameLen{};
	file.read((char*)&sceneNameLen, sizeof(int32_t));
	file.read(buf, sceneNameLen);
	scene.m_SceneName = buf;

	// Camera
	file.read((char*)&scene.m_Camera.m_AspectRatio, sizeof(float));
	file.read((char*)&scene.m_Camera.m_NearClip,	sizeof(float));
	file.read((char*)&scene.m_Camera.m_FarClip,		sizeof(float));
	file.read((char*)&scene.m_Camera.m_FOV,			sizeof(float));
	file.read((char*)&scene.m_Camera.m_Pitch,		sizeof(float));
	file.read((char*)&scene.m_Camera.m_Yaw,			sizeof(float));
	file.read((char*)&scene.m_Camera.m_Position,	sizeof(glm::vec3));
	scene.m_Camera.UpdateProjection();
	scene.m_Camera.UpdateView();

	TextureManager::ReInit();
	
	// Objects
	int32_t objectsCount{};
	file.read((char*)&objectsCount, sizeof(int32_t));

	for (int32_t i = 0; i < objectsCount; i++)
	{
		// Object info
		int32_t tagLen{};
		ObjectType type{};
		file.read((char*)&tagLen, sizeof(int32_t));
		file.read(buf, tagLen);
		file.read((char*)&type, sizeof(ObjectType));

		std::unique_ptr<Planet> planet = type == ObjectType::Planet ? std::make_unique<Planet>() : std::make_unique<Sun>();
		planet->m_Tag = buf;

		// Transform
		file.read((char*)&planet->m_Transform.Position, sizeof(glm::vec3));
		file.read((char*)&planet->m_Transform.Rotation, sizeof(glm::vec3));
		file.read((char*)&planet->m_Transform.Scale,	sizeof(glm::vec3));

		// Physics
		file.read((char*)&planet->m_Physics.LinearVelocity,	 sizeof(glm::vec3));
		file.read((char*)&planet->m_Physics.AngularVelocity, sizeof(glm::vec3));
		file.read((char*)&planet->m_Physics.Mass,			 sizeof(float));

		// Material
		file.read((char*)&planet->m_Material.Color,		sizeof(glm::vec4));
		file.read((char*)&planet->m_Material.Shininess, sizeof(float));
		file.read((char*)&planet->m_Material.Roughness, sizeof(float));

		// Textures
		int32_t albedoPathLen{};
		file.read((char*)&albedoPathLen, sizeof(int32_t));
		file.read(buf, albedoPathLen);
		std::string albedoPath = buf;

		int32_t normalPathLen{};
		file.read((char*)&normalPathLen, sizeof(int32_t));
		file.read(buf, normalPathLen);
		std::string normalPath = buf;

		int32_t specularPathLen{};
		file.read((char*)&specularPathLen, sizeof(int32_t));
		file.read(buf, specularPathLen);
		std::string specularPath = buf;

		if (albedoPath == "Default Albedo")
		{
			planet->m_Material.TextureID = TextureManager::GetTexture(TextureManager::DEFAULT_ALBEDO).value().TextureID;
		}
		else
		{
			planet->m_Material.TextureID = TextureManager::AddTexture(albedoPath)
				.value_or(TextureManager::GetTexture(TextureManager::DEFAULT_ALBEDO).value()).TextureID;
		}

		if (normalPath == "Default Normal")
		{
			planet->m_Material.NormalMapTextureID = TextureManager::GetTexture(TextureManager::DEFAULT_NORMAL).value().TextureID;
		}
		else
		{
			planet->m_Material.NormalMapTextureID = TextureManager::AddTexture(normalPath)
				.value_or(TextureManager::GetTexture(TextureManager::DEFAULT_NORMAL).value()).TextureID;
		}

		if (specularPath == "Default Specular")
		{
			planet->m_Material.SpecularMapTextureID = TextureManager::GetTexture(TextureManager::DEFAULT_SPECULAR).value().TextureID;
		}
		else
		{
			planet->m_Material.SpecularMapTextureID = TextureManager::AddTexture(specularPath)
				.value_or(TextureManager::GetTexture(TextureManager::DEFAULT_SPECULAR).value()).TextureID;
		}
		
		// If it's a star, point light info
		if (planet->m_Type == ObjectType::Sun)
		{
			Sun* sun = (Sun*)planet.get();
		
			file.read((char*)&sun->m_Light.Color,	  sizeof(glm::vec3));
			file.read((char*)&sun->m_Light.Intensity, sizeof(float));
		}

		scene.m_Planets.push_back(std::move(planet));
	}

	LOG_INFO("Successfully loaded scene \"{}\".", scene.m_SceneName);

	return scene;
}

bool SceneSerializer::SaveScene(const EditorScene& scene)
{
	FUNC_PROFILE();

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
	int32_t sceneNameLen = scene.m_SceneName.length() + 1;
	file.write((char*)&sceneNameLen, sizeof(int32_t));
	file.write(scene.m_SceneName.c_str(), sceneNameLen);

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
		int32_t tagLen = planet->m_Tag.length() + 1;
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
		int32_t albedoPathLen = albedoPath.length() + 1;
		file.write((char*)&albedoPathLen, sizeof(int32_t));
		file.write(albedoPath.c_str(), albedoPathLen);

		std::string normalPath = TextureManager::GetTexture(material.NormalMapTextureID)
			.value_or(TextureManager::GetTexture(TextureManager::DEFAULT_NORMAL).value()).Path;
		int32_t normalPathLen = normalPath.length() + 1;
		file.write((char*)&normalPathLen, sizeof(int32_t));
		file.write(normalPath.c_str(), normalPathLen);

		std::string specularPath = TextureManager::GetTexture(material.SpecularMapTextureID)
			.value_or(TextureManager::GetTexture(TextureManager::DEFAULT_SPECULAR).value()).Path;
		int32_t specularPathLen = specularPath.length() + 1;
		file.write((char*)&specularPathLen, sizeof(int32_t));
		file.write(specularPath.c_str(), specularPathLen);

		// If it's a star, point light info
		if (planet->m_Type == ObjectType::Sun)
		{
			Sun* sun = (Sun*)planet.get();

			file.write((char*)&sun->m_Light.Color,	   sizeof(glm::vec3));
			file.write((char*)&sun->m_Light.Intensity, sizeof(float));
		}
	}

	file.close();
	LOG_INFO("Saved scene {} to: {}", scene.m_SceneName, std::filesystem::absolute(writePath).string());

	return true;
}
