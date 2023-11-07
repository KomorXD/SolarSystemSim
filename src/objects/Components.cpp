#include "Components.hpp"

#include <imgui_dialog/ImGuiFileDialog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "../TextureManager.hpp"

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
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent(16.0f);
		ImGui::ColorEdit4("Color", glm::value_ptr(Color));
		ImGui::DragFloat("Shininess", &Shininess, 0.1f, 0.0f, 128.0f, "%.2f");

		TextureInfo tex = TextureManager::GetTexture(TextureID).value();
		float ratio = tex.Size.y / tex.Size.x;

		ImGui::InputText("Texture", tex.Path.data(), tex.Path.capacity(), ImGuiInputTextFlags_ReadOnly);
		float sizeY = ImGui::GetItemRectSize().y;
		ImGui::SameLine();

		if (ImGui::Button("...", ImVec2(sizeY, sizeY)))
		{
			ImGuiFileDialog::Instance()->OpenDialog("ChooseTextureKey", "Choose texture file",
				".png,.jpg,.jpeg,.bmp", ".", 1, nullptr, ImGuiFileDialogFlags_CaseInsensitiveExtention);
		}

		if (ImGuiFileDialog::Instance()->Display("ChooseTextureKey", ImGuiWindowFlags_NoCollapse, ImVec2(600.0f, 500.0f)))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				std::optional<TextureInfo> res = TextureManager::AddTexture(filePathName);

				if (res.has_value())
				{
					TextureID = res.value().TextureID;
				}
			}

			ImGuiFileDialog::Instance()->Close();
		}

		ImGui::Image((ImTextureID)TextureManager::GetAtlasTextureID(), ImVec2(256.0f, 256.0f * ratio),
			ImVec2(tex.UV.x, tex.UV.y), ImVec2(tex.UV.x + tex.Size.x, tex.UV.y + tex.Size.y));

		ImGui::Unindent(16.0f);
	}
}
