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
		ImGui::PrettyDragFloat3("Position", glm::value_ptr(Position));
		ImGui::PrettyDragFloat3("Rotation", glm::value_ptr(Rotation));
		ImGui::PrettyDragFloat3("Scale", glm::value_ptr(Scale), 1.0f);
		ImGui::Unindent(16.0f);
	}
}

void Physics::OnImGuiRender()
{
	if (ImGui::CollapsingHeader("Physics properties", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent(16.0f);
		ImGui::PrettyDragFloat3("Lin velocity", glm::value_ptr(LinearVelocity));
		ImGui::PrettyDragFloat3("Ang velocity", glm::value_ptr(AngularVelocity));
		ImGui::PrettyDragFloat("Mass", &Mass, 0.0f, FLT_MAX);
		ImGui::Unindent(16.0f);
	}
}

void Material::OnImGuiRender()
{
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent(16.0f);
		
		RenderAlbedo();

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();
		
		RenderNormal();

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();
		
		RenderSpecular();

		ImGui::Unindent(16.0f);
	}
}

void Material::RenderAlbedo()
{
	TextureInfo tex = TextureManager::GetTexture(TextureID).value();
					 
	ImGui::Text("Albedo");
	ImGui::NewLine();
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 100.0f);
	ImGui::Image((ImTextureID)TextureManager::GetAtlasTextureID(), ImVec2(64.0f, 64.0f),
		ImVec2(tex.UV.x, tex.UV.y), ImVec2(tex.UV.x + tex.Size.x, tex.UV.y + tex.Size.y));
	ImGui::NextColumn();
	ImGui::InputText("##TexturePath", tex.Path.data(), tex.Path.capacity(), ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine();
					 
	ImGui::PushID(1);
	if (ImGui::Button("...", ImVec2(22.0f, 22.0f)))
	{				 
		ImGuiFileDialog::Instance()->OpenDialog("ChooseTextureKey", "Choose texture file",
			"Image files (*.png *.gif *.jpg *.jpeg){.png,.gif,.jpg,.jpeg}, All files (*.*){.*}", ".", 1, nullptr, ImGuiFileDialogFlags_CaseInsensitiveExtention);
	}				 
	ImGui::PopID();	 
					 
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
					 
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(16.0f, 0.0f));
	ImGui::Checkbox("Use", &TextureInUse);
	ImGui::SameLine();
	ImGui::ColorEdit4("Color", glm::value_ptr(Color), ImGuiColorEditFlags_NoInputs);
	ImGui::Columns(1);
	ImGui::PopStyleVar();
}					 
					 
void Material::RenderNormal()
{
	TextureInfo tex = TextureManager::GetTexture(NormalMapTextureID).value();
					  
	ImGui::Text("Normal map");
	ImGui::NewLine(); 
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 100.0f);
	ImGui::Image((ImTextureID)TextureManager::GetAtlasTextureID(), ImVec2(64.0f, 64.0f),
		ImVec2(tex.UV.x, tex.UV.y), ImVec2(tex.UV.x + tex.Size.x, tex.UV.y + tex.Size.y));
	ImGui::NextColumn();
	ImGui::InputText("##NormalMapPath", tex.Path.data(), tex.Path.capacity(), ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine();
					  
	ImGui::PushID(2); 
	if (ImGui::Button("...", ImVec2(22.0f, 22.0f)))
	{				  
		ImGuiFileDialog::Instance()->OpenDialog("ChooseNormalTextureKey", "Choose normal map file",
			"Image files (*.png *.gif *.jpg *.jpeg){.png,.gif,.jpg,.jpeg}, All files (*.*){.*}", ".", 1, nullptr, ImGuiFileDialogFlags_CaseInsensitiveExtention);
	}				  
	ImGui::PopID();	  
					  
	if (ImGuiFileDialog::Instance()->Display("ChooseNormalTextureKey", ImGuiWindowFlags_NoCollapse, ImVec2(600.0f, 500.0f)))
	{				  
		if (ImGuiFileDialog::Instance()->IsOk())
		{			  
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::optional<TextureInfo> res = TextureManager::AddTexture(filePathName);
					  
			if (res.has_value())
			{		  
				NormalMapTextureID = res.value().TextureID;
			}		  
		}			  
					  
		ImGuiFileDialog::Instance()->Close();
	}				

	//ImGui::NewLine();
	//ImGui::DragFloat("Roughness", &Roughness, 0.1f, 0.0f, 1.0f, "%.2f");
	ImGui::Columns(1);
}

void Material::RenderSpecular()
{
	TextureInfo tex = TextureManager::GetTexture(SpecularMapTextureID).value();

	ImGui::Text("Specular map");
	ImGui::NewLine();
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 100.0f);
	ImGui::Image((ImTextureID)TextureManager::GetAtlasTextureID(), ImVec2(64.0f, 64.0f),
		ImVec2(tex.UV.x, tex.UV.y), ImVec2(tex.UV.x + tex.Size.x, tex.UV.y + tex.Size.y));
	ImGui::NextColumn();
	ImGui::InputText("##SpecularMapPath", tex.Path.data(), tex.Path.capacity(), ImGuiInputTextFlags_ReadOnly);
	ImGui::SameLine();

	ImGui::PushID(3);
	if (ImGui::Button("...", ImVec2(22.0f, 22.0f)))
	{
		ImGuiFileDialog::Instance()->OpenDialog("ChooseSpecularTextureKey", "Choose specular map file",
			"Image files (*.png *.gif *.jpg *.jpeg){.png,.gif,.jpg,.jpeg}, All files (*.*){.*}", ".", 1, nullptr, ImGuiFileDialogFlags_CaseInsensitiveExtention);
	}
	ImGui::PopID();

	if (ImGuiFileDialog::Instance()->Display("ChooseSpecularTextureKey", ImGuiWindowFlags_NoCollapse, ImVec2(600.0f, 500.0f)))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::optional<TextureInfo> res = TextureManager::AddTexture(filePathName);

			if (res.has_value())
			{
				SpecularMapTextureID = res.value().TextureID;
			}
		}

		ImGuiFileDialog::Instance()->Close();
	}

	ImGui::NewLine();
	ImGui::DragFloat("Shininess", &Shininess, 0.1f, 1.0f, 128.0f);
	ImGui::Columns(1);
}

void PointLight::OnImGuiRender()
{
	if (ImGui::CollapsingHeader("Point light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent(16.0f);
		ImGui::ColorEdit4("Light color", glm::value_ptr(Color));
		ImGui::PrettyDragFloat("Intensity", &Intensity, 0.0f, FLT_MAX);
		ImGui::Unindent(16.0f);
	}
}
