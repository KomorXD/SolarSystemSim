#pragma once

#include <string>
#include <optional>
#include <vector>

#include "stb_rect_pack/stb_rect_pack.h"
#include "OpenGL.hpp"

struct TextureInfo
{
	int32_t TextureID;
	std::string Path;
	glm::ivec2 UV;
	glm::ivec2 Size;
};

class TextureManager
{
public:
	static void Init();

	static TextureInfo AddTexture(const std::string& path);

	static std::optional<TextureInfo> GetTexture(const std::string& path);
	static std::optional<TextureInfo> GetTexture(uint32_t id);
	
	static bool RemoveTexture(const std::string& path);
	static bool RemoveTexture(uint32_t id);

private:
	TextureManager() = default;

	static std::unique_ptr<Texture> s_Atlas;
	static std::vector<TextureInfo> s_Textures;
	static int32_t s_IndexCounter;

	static stbrp_context s_Context;
	static std::vector<stbrp_node> s_Nodes;
	static std::vector<stbrp_rect> s_Rects;
};