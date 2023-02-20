#include "Input.hpp"
#include "Application.hpp"

#include <glfw/glfw3.h>
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
