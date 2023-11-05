#include "TextureManager.hpp"
#include "OpenGL.hpp"

std::vector<TextureInfo> TextureManager::s_Textures;
int32_t TextureManager::s_IndexCounter = 1;

TextureInfo TextureManager::AddTexture(const std::string& path)
{
	auto texItr = std::find_if(s_Textures.begin(), s_Textures.end(), [&](const TextureInfo& tex) { return tex.Path == path; });

	if (texItr != s_Textures.end())
	{
		return *texItr;
	}

	// add to atlas

	s_Textures.push_back({ s_IndexCounter, path });
	s_IndexCounter++;

	return s_Textures.back();
}

std::optional<TextureInfo> TextureManager::GetTexture(const std::string& path)
{
	auto texItr = std::find_if(s_Textures.begin(), s_Textures.end(), [&](const TextureInfo& tex) { return tex.Path == path; });

	if (texItr == s_Textures.end())
	{
		return {};
	}

	return *texItr;
}

std::optional<TextureInfo> TextureManager::GetTexture(uint32_t id)
{
	auto texItr = std::find_if(s_Textures.begin(), s_Textures.end(), [&](const TextureInfo& tex) { return tex.TextureID == id; });

	if (texItr == s_Textures.end())
	{
		return {};
	}

	return *texItr;
}

bool TextureManager::RemoveTexture(const std::string& path)
{
	auto texItr = std::find_if(s_Textures.begin(), s_Textures.end(), [&](const TextureInfo& tex) { return tex.Path == path; });

	if (texItr == s_Textures.end())
	{
		return false;
	}

	s_Textures.erase(texItr);

	return true;
}

bool TextureManager::RemoveTexture(uint32_t id)
{
	auto texItr = std::find_if(s_Textures.begin(), s_Textures.end(), [&](const TextureInfo& tex) { return tex.TextureID == id; });

	if (texItr == s_Textures.end())
	{
		return false;
	}

	s_Textures.erase(texItr);

	return true;
}
