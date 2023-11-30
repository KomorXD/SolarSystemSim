#pragma once

#include <string>
#include <optional>

class EditorScene;

class SceneSerializer
{
public:
	static std::optional<EditorScene> LoadScene(const std::string& path);
	static bool SaveScene(const EditorScene& scene);

private:
	SceneSerializer() = default;
};