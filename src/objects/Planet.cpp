#include "Planet.hpp"
#include "../Application.hpp"
#include "../Simulator.hpp"
#include "../TextureManager.hpp"

#include <imgui/imgui.h>
#include <glm/gtc/constants.hpp>

Planet::Planet()
	: SceneObject()
{
	m_Type = ObjectType::Planet;
}

Planet::Planet(const Planet& other)
{
	m_Tag		= other.m_Tag;
	m_ObjectID	= other.m_ObjectID;
	m_Type		= other.m_Type;
	m_Transform = other.m_Transform;
	m_Physics	= other.m_Physics;
	m_Material	= other.m_Material;
}

void Planet::OnUpdate(float ts)
{
}

void Planet::OnTick()
{
	glm::vec3 move = m_Physics.LinearVelocity * Application::TPS_STEP * Application::TPS_MULTIPLIER;

	m_Transform.Position += move * (20.0f * glm::pi<float>() / 365.0f);
}

void Planet::OnConfigRender()
{
	ImGui::BeginChild("Planet settings");
	
	ImGui::Text("Planet #%d", m_ObjectID);
	ImGui::Indent(16.0f);
	
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 100.0f);
	ImGui::Text("Tag");
	ImGui::NextColumn();
	ImGui::PushItemWidth(ImGui::CalcItemWidth());

	char buf[24];
	strcpy_s(buf, 24, m_Tag.data());
	ImGui::InputText("##Planet tag", buf, 24);
	m_Tag = buf;

	ImGui::Columns(1);
	ImGui::PopItemWidth();
	ImGui::Unindent(16.0f);

	ImGui::NewLine();
	m_Transform.OnImGuiRender();

	ImGui::NewLine();
	m_Physics.OnImGuiRender();

	ImGui::NewLine();
	m_Material.OnImGuiRender();
		
	ImGui::EndChild();
}

void Planet::OnSimDataRender()
{
	ImGui::SetNextWindowSize({ 512.0f, 640.0f }, ImGuiCond_FirstUseEver);
	ImGui::Begin(m_Tag.c_str());

	if (ImGui::CollapsingHeader("Object state", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent(16.0f);

		if (ImGui::BeginTable("Potemxd", 2, ImGuiTableFlags_Borders))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("Position [X, Y, Z]");
			ImGui::TableNextColumn();
			ImGui::Text("[%f, %f, %f]", m_Transform.Position.x, m_Transform.Position.y, m_Transform.Position.z);

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("Velocity [X, Y, Z] [km/s]");
			ImGui::TableNextColumn();
			ImGui::Text("[%f, %f, %f]", m_Physics.LinearVelocity.x, m_Physics.LinearVelocity.y, m_Physics.LinearVelocity.z);

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("Velocity [km/s]");
			ImGui::TableNextColumn();
			ImGui::Text("%f", glm::length(m_Physics.LinearVelocity));

			ImGui::EndTable();
		}

		ImGui::NewLine();
		ImGui::PrettyDragFloat("Mass [* sun's mass]", &m_Physics.Mass, 0.0f, FLT_MAX, 150.0f);

		ImGui::Unindent(16.0f);
	}

	ImGui::NewLine();

	if (ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent(16.0f);

		if (ImGui::BeginTable("Textures", 3))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("Albedo");
			ImGui::TableNextColumn();
			ImGui::Text("Normal");
			ImGui::TableNextColumn();
			ImGui::Text("Specular");

			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			TextureInfo tex = TextureManager::GetTexture(m_Material.TextureID).value_or(
				TextureManager::GetTexture(TextureManager::DEFAULT_ALBEDO).value()
			);
			ImGui::PushID(1);
			ImGui::Image((ImTextureID)TextureManager::GetAtlasTextureID(), ImVec2(64.0f, 64.0f),
				ImVec2(tex.UV.x, tex.UV.y), ImVec2(tex.UV.x + tex.Size.x, tex.UV.y + tex.Size.y));
			ImGui::PopID();

			ImGui::TableNextColumn();
			tex = TextureManager::GetTexture(m_Material.NormalMapTextureID).value_or(
				TextureManager::GetTexture(TextureManager::DEFAULT_NORMAL).value()
			);
			ImGui::PushID(2);
			ImGui::Image((ImTextureID)TextureManager::GetAtlasTextureID(), ImVec2(64.0f, 64.0f),
				ImVec2(tex.UV.x, tex.UV.y), ImVec2(tex.UV.x + tex.Size.x, tex.UV.y + tex.Size.y));
			ImGui::PopID();

			ImGui::TableNextColumn();
			tex = TextureManager::GetTexture(m_Material.SpecularMapTextureID).value_or(
				TextureManager::GetTexture(TextureManager::DEFAULT_SPECULAR).value()
			);
			ImGui::PushID(3);
			ImGui::Image((ImTextureID)TextureManager::GetAtlasTextureID(), ImVec2(64.0f, 64.0f),
				ImVec2(tex.UV.x, tex.UV.y), ImVec2(tex.UV.x + tex.Size.x, tex.UV.y + tex.Size.y));
			ImGui::PopID();

			ImGui::EndTable();
		}

		ImGui::Unindent(16.0f);
	}

	ImGui::End();
}

std::unique_ptr<Planet> Planet::Clone()
{
	auto ptr = std::make_unique<Planet>();

	ptr->GetTransform() = m_Transform;
	ptr->GetPhysics()   = m_Physics;
	ptr->GetMaterial()  = m_Material;

	return ptr;
}

void Planet::SetRelativePlanet(Planet* planet)
{
	m_RelativePathPlanet = planet;
}
