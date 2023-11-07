#include "Components.hpp"

#include <imgui_dialog/ImGuiFileDialog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

glm::mat4 Transform::Matrix() const
{
	return glm::translate(glm::mat4(1.0f), Position)
		* glm::scale(glm::mat4(1.0f), Scale)
		* glm::toMat4(glm::quat(Rotation));
}

void Transform::OnImGuiRender()
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent(16.0f);
		ImGui::DragFloat3("Position", glm::value_ptr(Position), 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
		ImGui::DragFloat3("Rotation", glm::value_ptr(Rotation), 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
		ImGui::DragFloat3("Scale", glm::value_ptr(Scale), 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
		ImGui::Unindent(16.0f);
	}
}

void Physics::OnImGuiRender()
{
	if (ImGui::CollapsingHeader("Physics properties", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent(16.0f);
		ImGui::DragFloat3("Linear velocity", glm::value_ptr(LinearVelocity), 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
		ImGui::DragFloat3("Angular velocity", glm::value_ptr(AngularVelocity), 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
		ImGui::DragFloat("Mass", &Mass, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
		ImGui::Unindent(16.0f);
	}
}

void Material::OnImGuiRender()
{
}
