#include "TextureManager.hpp"
#include "stb_rect_pack/stb_rect_pack.h"
#include "stb_image/stb_image.h"
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
	s_Nodes.resize(256);

	AddDefaults();
	s_IndexCounter = DEFAULT_NORMAL + 1;

	AddTexture("res/textures/icons/new-planet.png");
	AddTexture("res/textures/icons/new-sun.png");
}

void TextureManager::BindAtlas(uint32_t slot)
{
	s_Atlas->Bind(slot);
}

std::optional<TextureInfo> TextureManager::AddTexture(const std::string& path)
{
	auto texItr = std::find_if(s_Textures.begin(), s_Textures.end(), [&](const TextureInfo& tex) { return tex.Path == path; });

	if (texItr != s_Textures.end())
	{
		return *texItr;
	}

	stbi_set_flip_vertically_on_load(0);
	int32_t width{}, height{}, bpp{};
	uint8_t* buffer = stbi_load(path.c_str(), &width, &height, &bpp, 4);

	if (buffer == nullptr)
	{
		return {};
	}

	// ===
	stbrp_rect texRect{};
	texRect.id = s_IndexCounter;
	texRect.w = width;
	texRect.h = height;
	s_Rects.push_back(texRect);
	s_IndexCounter++;

	if (stbrp_pack_rects(&s_Context, s_Rects.data(), s_Rects.size()))
	{
		LOG_INFO("Added texture");
	}
	else
	{
		LOG_ERROR("Texture did not fit");
	}

	texRect = s_Rects.back();

	s_Atlas->SetSubtexture(buffer, { texRect.x, texRect.y }, { texRect.w, texRect.h });
	s_Textures.push_back({ texRect.id, path, { texRect.x / 4096.0f, texRect.y / 4096.0f },
		{ texRect.w / 4096.0f, texRect.h / 4096.0f } });
	// ===

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

void TextureManager::AddDefaults()
{
	stbrp_rect rect{};
	rect.id = DEFAULT_ALBEDO;
	rect.w = 1;
	rect.h = 1;
	s_Rects.push_back(rect);
	
	rect.id = DEFAULT_NORMAL;
	rect.w = 256;
	rect.h = 256;
	s_Rects.push_back(rect);

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
	rect = s_Rects[0];

	s_Atlas->SetSubtexture(whitePixelData, { rect.x, rect.y }, { rect.w, rect.h });
	s_Textures.push_back({ rect.id, "", { rect.x / 4096.0f, rect.y / 4096.0f },
		{ rect.w / 4096.0f, rect.h / 4096.0f } });

	std::vector<glm::vec4> normalMapPixelData(256 * 256, { 0.0f, 0.0f, 255.0f, 255.0f });
	rect = s_Rects[1];
	s_Atlas->SetSubtexture((const uint8_t*)normalMapPixelData.data(), { rect.x, rect.y }, { rect.w, rect.h });
	s_Textures.push_back({ rect.id, "", { rect.x / 4096.0f, rect.y / 4096.0f },
		{ rect.w / 4096.0f, rect.h / 4096.0f } });
}
