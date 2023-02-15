#pragma once

#include <glm/glm.hpp>

enum class MouseButton
{
	Left = 0,
	Right = 1,
	Middle = 2
};

enum class Key
{
	A = 65,
	Escape = 256,
	LeftShift = 340,
	LeftCtrl = 341,
	LeftAlt = 342
};

class Input
{
public:
	static bool IsKeyPressed(Key key);

	static bool IsMouseButtonPressed(MouseButton button);
	static glm::vec2 GetMousePosition();
};