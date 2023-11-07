#include "Planet.hpp"
#include "../TextureManager.hpp"

#include <imgui/imgui.h>
#include <imgui_dialog/ImGuiFileDialog.h>
#include <glm/gtc/type_ptr.hpp>

static uint32_t s_IdCounter = 1;

PlanetaryObject::PlanetaryObject(const glm::vec3& position)
{
	m_EntityID = s_IdCounter;
	m_Tag.reserve(24);
	m_Tag = std::string("Planet #") + std::to_string(m_EntityID);
	++s_IdCounter;

	m_Transform.Position = position;
}

void PlanetaryObject::OnUpdate(float ts)
{
	m_Transform.Position += m_Physics.LinearVelocity * ts;
}

void PlanetaryObject::Move(const glm::vec3& offset)
{
	m_Transform.Position += offset;
}

void PlanetaryObject::SetPosition(const glm::vec3& position)
{
	m_Transform.Position = position;
}

void PlanetaryObject::SetScale(const glm::vec3& scale)
{
	m_Transform.Scale = scale;
}

void PlanetaryObject::SetRadius(float radius)
{
	m_Transform.Scale = glm::vec3(radius);
}

void PlanetaryObject::SetRotation(const glm::vec3& rotation)
{
	m_Transform.Rotation = rotation;
}

void PlanetaryObject::SetMass(float mass)
{
	m_Physics.Mass = mass;
}

void PlanetaryObject::SetTextureID(int32_t id)
{
	m_Material.TextureID = id;
}

void PlanetaryObject::SetVelocity(const glm::vec3& velocity)
{
	m_Physics.LinearVelocity = velocity;
}

void PlanetaryObject::AddVelocity(const glm::vec3& addVelocity)
{
	m_Physics.LinearVelocity += addVelocity;
}

void PlanetaryObject::OnConfigRender()
{
	ImGui::BeginChild("Sphere settings");

	ImGui::Text("Planet #%d", m_EntityID);
	ImGui::Indent(16.0f);
	ImGui::InputText("##Planet tag", m_Tag.data(), m_Tag.capacity());
	ImGui::Unindent(16.0f);
	ImGui::NewLine();

	m_Transform.OnImGuiRender();
	ImGui::NewLine();

	m_Physics.OnImGuiRender();
	ImGui::NewLine();
	
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent(16.0f);
		ImGui::ColorEdit4("Color", glm::value_ptr(m_Material.Color));
		ImGui::DragFloat("Shininess Color", &m_Material.Shininess, 0.1f, 0.0f, 128.0f, "%.2f");

		TextureInfo tex = TextureManager::GetTexture(m_Material.TextureID).value();
		float ratio = tex.Size.y / tex.Size.x;

		ImGui::Text("Texture");
		ImGui::InputText("##TexturePath", tex.Path.data(), tex.Path.capacity(), ImGuiInputTextFlags_ReadOnly);
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
					m_Material.TextureID = res.value().TextureID;
				}
			}

			ImGuiFileDialog::Instance()->Close();
		}

		ImGui::Image((ImTextureID)TextureManager::GetAtlasTextureID(), ImVec2(256.0f, 256.0f * ratio),
			ImVec2(tex.UV.x, tex.UV.y), ImVec2(tex.UV.x + tex.Size.x, tex.UV.y + tex.Size.y));

		ImGui::Unindent(16.0f);
		ImGui::NewLine();
	}
	
	ImGui::EndChild();
}
