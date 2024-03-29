#include "Sun.hpp"

#include <imgui/imgui.h>

Sun::Sun()
	: Planet()
{
	m_Type = ObjectType::Sun;
}

Sun::Sun(const Sun& other)
{
	m_Tag		= other.m_Tag;
	m_ObjectID	= other.m_ObjectID;
	m_Type		= other.m_Type;
	m_Transform = other.m_Transform;
	m_Physics	= other.m_Physics;
	m_Material	= other.m_Material;
	m_Light		= other.m_Light;
}

void Sun::OnConfigRender()
{
	ImGui::BeginChild("Sun settings");

	ImGui::Text("Planet #%d", m_ObjectID);
	ImGui::Indent(16.0f);
	ImGui::InputText("##Planet tag", m_Tag.data(), m_Tag.capacity());
	ImGui::Unindent(16.0f);
	ImGui::NewLine();

	m_Transform.OnImGuiRender();
	ImGui::NewLine();

	m_Physics.OnImGuiRender();
	ImGui::NewLine();

	m_Material.OnImGuiRender();
	ImGui::NewLine();

	m_Light.OnImGuiRender();
	ImGui::NewLine();

	ImGui::EndChild();
}

std::unique_ptr<Planet> Sun::Clone()
{
	auto ptr = std::make_unique<Sun>();
	
	ptr->GetTransform() = m_Transform;
	ptr->GetPhysics()   = m_Physics;
	ptr->GetMaterial()  = m_Material;
	ptr->GetLight()		= m_Light;

	return ptr;
}
