#pragma once

#include "Input.hpp"

#include <queue>

struct Event
{
	struct ResizeEvent
	{
		uint32_t Width;
		uint32_t Height;
	};

	struct KeyboardEvent
	{
		Key  Code;
		bool AltPressed;
		bool ShiftPressed;
		bool CtrlPressed;
	};

	struct MouseButtonEvent
	{
		MouseButton Button;
	};

	struct MouseScrollEvent
	{
		float OffsetX;
		float OffsetY;
	};

	enum EventType
	{
		None,
		WindowResized,
		KeyPressed,
		MouseButtonPressed,
		MouseWheelScrolled
	};

	EventType Type = None;

	union
	{
		ResizeEvent Size;
		KeyboardEvent Key;
		MouseButtonEvent MouseButton;
		MouseScrollEvent MouseWheel;
	};
};

class EventQueue
{
public:
	bool PollEvents(Event& ev);

	void SubmitEvent(Event& ev);

private:
	std::queue<Event> m_Events;
};