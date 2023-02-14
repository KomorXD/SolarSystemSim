#include "Input.hpp"
#include "Application.hpp"

#include <glfw/glfw3.h>

bool Input::IsKeyPressed(Keyboard key)
{
	Application* app = Application::GetInstance();

	return glfwGetKey(app->GetWindow(), (int32_t)key) == GLFW_PRESS;
}

bool Input::IsMouseButtonPressed(MouseButton button)
{
	Application* app = Application::GetInstance();
	
	return glfwGetMouseButton(app->GetWindow(), (int32_t)button) == GLFW_PRESS;
}
