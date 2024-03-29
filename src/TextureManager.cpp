#include "TextureManager.hpp"
#include "stb_rect_pack/stb_rect_pack.h"
#include "stb_image/stb_image.h"
#include "Logger.hpp"
#include "Timer.hpp"

#include <filesystem>

std::unique_ptr<Texture> TextureManager::s_Atlas;
std::vector<TextureInfo> TextureManager::s_Textures;
int32_t TextureManager::s_IndexCounter = 1;

stbrp_context TextureManager::s_Context;
std::vector<stbrp_rect> TextureManager::s_Rects;
std::vector<stbrp_node> TextureManager::s_Nodes;

void TextureManager::Init()
{
	FUNC_PROFILE();

	s_Atlas = std::make_unique<Texture>(2048, 2048);
	s_Nodes.resize(256);

	AddDefaults();
	s_IndexCounter = DEFAULT_SPECULAR + 1;

	AddTexture("res/textures/icons/new-planet.png");
	AddTexture("res/textures/icons/new-sun.png");
	AddTexture("res/textures/icons/play.png");
	AddTexture("res/textures/icons/pause.png");
	AddTexture("res/textures/icons/stop.png");
	AddTexture("res/textures/icons/settings.png");
}

void TextureManager::ReInit()
{
	s_Rects.clear();
	s_Nodes.clear();
	s_Textures.clear();

	Init();
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

	stbrp_rect texRect{};
	texRect.id = s_IndexCounter;
	texRect.w = width;
	texRect.h = height;
	s_Rects.push_back(texRect);
	s_IndexCounter++;

	if (stbrp_pack_rects(&s_Context, s_Rects.data(), s_Rects.size()))
	{
		LOG_INFO("Added texture");

		texRect = s_Rects.back();

		s_Atlas->SetSubtexture(buffer, { texRect.x, texRect.y }, { texRect.w, texRect.h });
		s_Textures.push_back({ texRect.id, std::filesystem::relative(path).string(),
			{ texRect.x / (float)s_Atlas->GetWidth(), texRect.y / (float)s_Atlas->GetHeight() },
			{ texRect.w / (float)s_Atlas->GetWidth(), texRect.h / (float)s_Atlas->GetHeight() } });

		stbi_image_free(buffer);
	}
	else
	{
		s_Textures.push_back({ texRect.id, std::filesystem::relative(path).string(),
			{ texRect.x / (float)s_Atlas->GetWidth(), texRect.y / (float)s_Atlas->GetHeight() },
			{ texRect.w / (float)s_Atlas->GetWidth(), texRect.h / (float)s_Atlas->GetHeight() } });

		GrowAtlas();
	}

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
	rect.w = 1;
	rect.h = 1;
	s_Rects.push_back(rect);
	
	rect.id = DEFAULT_SPECULAR;
	rect.w = 1;
	rect.h = 1;
	s_Rects.push_back(rect);

	stbrp_init_target(&s_Context, (uint32_t)s_Atlas->GetWidth(), (uint32_t)s_Atlas->GetHeight(), s_Nodes.data(), s_Nodes.size());

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
	s_Textures.push_back({ rect.id, "Default Albedo", 
		{ rect.x / (float)s_Atlas->GetWidth(), rect.y / (float)s_Atlas->GetHeight() },
		{ rect.w / (float)s_Atlas->GetWidth(), rect.h / (float)s_Atlas->GetHeight() } });

	uint8_t normalMapPixelData[4] = { 127, 127, 255, 255 };
	rect = s_Rects[1];
	s_Atlas->SetSubtexture(normalMapPixelData, { rect.x, rect.y }, { rect.w, rect.h });
	s_Textures.push_back({ rect.id, "Default Normal", 
		{ rect.x / (float)s_Atlas->GetWidth(), rect.y / (float)s_Atlas->GetHeight() },
		{ rect.w / (float)s_Atlas->GetWidth(), rect.h / (float)s_Atlas->GetHeight() } });
	
	rect = s_Rects[2];
	s_Atlas->SetSubtexture(whitePixelData, { rect.x, rect.y }, { rect.w, rect.h });
	s_Textures.push_back({ rect.id, "Default Specular", 
		{ rect.x / (float)s_Atlas->GetWidth(), rect.y / (float)s_Atlas->GetHeight() },
		{ rect.w / (float)s_Atlas->GetWidth(), rect.h / (float)s_Atlas->GetHeight() } });
}

void TextureManager::GrowAtlas()
{
	std::vector<stbrp_rect> rectsCopy = s_Rects;
	
	do
	{
		uint32_t newWidth = s_Atlas->GetWidth() * 2;
		uint32_t newHeight = s_Atlas->GetHeight() * 2;

		s_Nodes.clear();
		s_Nodes.resize(256);

		delete s_Atlas.release();
		s_Atlas = std::make_unique<Texture>(newWidth, newHeight);
		
		stbrp_init_target(&s_Context, s_Atlas->GetWidth(), s_Atlas->GetHeight(), s_Nodes.data(), s_Nodes.size());
	} while (!stbrp_pack_rects(&s_Context, s_Rects.data(), s_Rects.size()));

	LOG_INFO("Atlas resized to {}x{}", s_Atlas->GetWidth(), s_Atlas->GetHeight());

	stbrp_rect rect{};
	uint8_t whitePixelData[4] = { 255, 255, 255, 255 };
	rect = *std::find_if(s_Rects.begin(), s_Rects.end(),
		[](const stbrp_rect& rect)
		{ 
			return rect.id == DEFAULT_ALBEDO; 
		});
	s_Atlas->SetSubtexture(whitePixelData, { rect.x, rect.y }, { rect.w, rect.h });
	s_Textures[0] = { rect.id, "Default Albedo",
		{ rect.x / (float)s_Atlas->GetWidth(), rect.y / (float)s_Atlas->GetHeight() },
		{ rect.w / (float)s_Atlas->GetWidth(), rect.h / (float)s_Atlas->GetHeight() } };

	uint8_t normalMapPixelData[4] = { 127, 127, 255, 255 };
	rect = *std::find_if(s_Rects.begin(), s_Rects.end(),
		[](const stbrp_rect& rect)
		{
			return rect.id == DEFAULT_NORMAL;
		});
	s_Atlas->SetSubtexture(normalMapPixelData, { rect.x, rect.y }, { rect.w, rect.h });
	s_Textures[1] = { rect.id, "Default Normal",
		{ rect.x / (float)s_Atlas->GetWidth(), rect.y / (float)s_Atlas->GetHeight() },
		{ rect.w / (float)s_Atlas->GetWidth(), rect.h / (float)s_Atlas->GetHeight() } };

	rect = *std::find_if(s_Rects.begin(), s_Rects.end(),
		[](const stbrp_rect& rect)
		{
			return rect.id == DEFAULT_SPECULAR;
		});
	s_Atlas->SetSubtexture(whitePixelData, { rect.x, rect.y }, { rect.w, rect.h });
	s_Textures[2] = { rect.id, "Default Specular",
		{ rect.x / (float)s_Atlas->GetWidth(), rect.y / (float)s_Atlas->GetHeight() },
		{ rect.w / (float)s_Atlas->GetWidth(), rect.h / (float)s_Atlas->GetHeight() } };

	for (TextureInfo& tex : s_Textures)
	{
		if (tex.TextureID < 4)
		{
			continue;
		}

		stbi_set_flip_vertically_on_load(0);
		int32_t width{}, height{}, bpp{};
		uint8_t* buffer = stbi_load(tex.Path.c_str(), &width, &height, &bpp, 4);

		if (buffer == nullptr)
		{
			continue;
		}

		stbrp_rect texRect = *std::find_if(s_Rects.begin(), s_Rects.end(),
			[&tex](const stbrp_rect& rect)
			{
				return rect.id == tex.TextureID;
			});
		s_Atlas->SetSubtexture(buffer, { texRect.x, texRect.y }, { texRect.w, texRect.h });
		tex.Size = { texRect.w / (float)s_Atlas->GetWidth(), texRect.h / (float)s_Atlas->GetHeight() };
		tex.UV = { texRect.x / (float)s_Atlas->GetWidth(), texRect.y / (float)s_Atlas->GetHeight() };

		stbi_image_free(buffer);
	}
}
