#pragma once

#include <string>
#include <optional>
#include <vector>

struct TextureInfo
{
	int32_t TextureID;
	std::string Path;
	// glm::vec2 UV;
	// glm::vec2 Size;
};

class TextureManager
{
public:
	TextureInfo AddTexture(const std::string& path);

	std::optional<TextureInfo> GetTexture(const std::string& path);
	std::optional<TextureInfo> GetTexture(uint32_t id);
	
	bool RemoveTexture(const std::string& path);
	bool RemoveTexture(uint32_t id);

private:
	TextureManager() = default;

	// static Texture s_Atlas;
	static std::vector<TextureInfo> s_Textures;
	static int32_t s_IndexCounter;
};