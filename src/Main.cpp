#include <iostream>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <spdlog/spdlog.h>

int main(int argc, char** argv)
{
	if (glfwInit() == GLFW_FALSE)
	{
		std::cerr << "ahaaaaa\n";

		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* w = glfwCreateWindow(1280, 720, "XDDDD", NULL, NULL);

	glfwMakeContextCurrent(w);
	glfwSwapInterval(0);

	if (!gladLoadGL())
	{
		std::cerr << "serrrr\n";

		return 1;
	}

	glm::vec3 lol(2.0f);

	printf("%f %f %f\n", lol.x, lol.y, lol.z);

	std::cout << ":3c\n";

	spdlog::error("SIEMAAAAAAAAAAAAAAAAAAAAA");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(w, true);
	ImGui_ImplOpenGL3_Init();

	while (!glfwWindowShouldClose(w))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		ImGui::Begin("Siema");

		if (ImGui::Button("LOOOOOOOOOL"))
		{
			spdlog::critical("HALOOO");
		}

		ImGui::End();

		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwPollEvents();
		glfwSwapBuffers(w);
	}

	return 0;
}