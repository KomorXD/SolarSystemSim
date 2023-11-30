#pragma once

#include <string>

class EditorScene;

class SceneSerializer
{
public:
	static EditorScene LoadScene(const std::string& path);
	static bool SaveScene(const EditorScene& scene);

private:
	SceneSerializer() = default;
};