#include "TextureManager.hpp"
#include "stb_rect_pack/stb_rect_pack.h"
#include "stb_image/stb_image_write.h"
#include "Logger.hpp"
#include "Timer.hpp"

std::unique_ptr<Texture> TextureManager::s_Atlas;
std::vector<TextureInfo> TextureManager::s_Textures;
int32_t TextureManager::s_IndexCounter = 1;

stbrp_context TextureManager::s_Context;
std::vector<stbrp_rect> TextureManager::s_Rects;
std::vector<stbrp_node> TextureManager::s_Nodes;

void TextureManager::Init()
{
	FUNC_PROFILE();

	s_Atlas = std::make_unique<Texture>(4096, 4096);
	s_Nodes.resize(255);

	stbrp_rect whitePixelRect{};
	whitePixelRect.id = s_IndexCounter;
	whitePixelRect.w = 1;
	whitePixelRect.h = 1;
	s_Rects.push_back(whitePixelRect);
	s_IndexCounter++;

	stbrp_init_target(&s_Context, 4096, 4096, s_Nodes.data(), s_Nodes.size());

	if (stbrp_pack_rects(&s_Context, s_Rects.data(), s_Rects.size()))
	{
		LOG_INFO("Initialized stbrp_rect_pack.");
	}
	else
	{
		LOG_ERROR("A 1x1 pixel somehow didn't fit xd");
	}

	uint8_t whitePixelData[4] = { 255, 255, 255, 255 };
	whitePixelRect = s_Rects[0];

	s_Atlas->SetSubtexture(whitePixelData, { whitePixelRect.x, whitePixelRect.y }, { whitePixelRect.w, whitePixelRect.h });
	s_Textures.push_back({ whitePixelRect.id, "", { whitePixelRect.x, whitePixelRect.y }, { whitePixelRect.w, whitePixelRect.h } });
}

void TextureManager::BindAtlas(uint32_t slot)
{
	s_Atlas->Bind(slot);
}

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
