#include "Planet.hpp"
#include "../Application.hpp"

#include <imgui/imgui.h>

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
	m_Transform.Position += m_Physics.LinearVelocity * Application::TPS_STEP * Application::TPS_MULTIPLIER;
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
	ImGui::InputText("##Planet tag", m_Tag.data(), m_Tag.capacity());
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
	ImGui::SetNextWindowSize({ 500.0f, 312.0f }, ImGuiCond_FirstUseEver);
	ImGui::Begin(m_Tag.c_str());

	m_Transform.OnImGuiRender();
	ImGui::NewLine();
	m_Physics.OnImGuiRender();

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
