#pragma once

#include <stdint.h>

enum class MouseButton
{
	Left = 0,
	Right = 1,
	Middle = 2
};

enum class Keyboard
{
	Escape = 256
};

class Input
{
public:
	static bool IsKeyPressed(Keyboard key);

	static bool IsMouseButtonPressed(MouseButton button);
};