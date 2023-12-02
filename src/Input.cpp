#include "Input.hpp"
#include "Application.hpp"
#include "renderer/Renderer.hpp"

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

bool Input::IsKeyPressed(Key key)
{
	Application* app = Application::GetInstance();

	return glfwGetKey(app->GetWindow(), (int32_t)key) == GLFW_PRESS;
}

bool Input::IsMouseButtonPressed(MouseButton button)
{
	Application* app = Application::GetInstance();
	
	return glfwGetMouseButton(app->GetWindow(), (int32_t)button) == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition()
{
	Application* app = Application::GetInstance();
	double xPos{};
	double yPos{};

	glfwGetCursorPos(app->GetWindow(), &xPos, &yPos);

	return { (float)xPos, (float)app->GetWindowSpec().Height - (float)yPos };
}

void Input::HideCursor()
{
	Application* app = Application::GetInstance();

	glfwSetInputMode(app->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Input::DisableCursor()
{
	Application* app = Application::GetInstance();

	glfwSetInputMode(app->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Input::ShowCursor()
{
	Application* app = Application::GetInstance();

	glfwSetInputMode(app->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
