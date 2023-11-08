#include "Planet.hpp"

#include <imgui/imgui.h>

Planet::Planet()
	: SceneObject()
{
	m_Type = ObjectType::Planet;
}

void Planet::OnUpdate(float ts)
{
	m_Transform.Position += m_Physics.LinearVelocity * ts;
}

void Planet::OnTick()
{
}

void Planet::OnConfigRender()
{
	ImGui::BeginChild("Planet settings");
	
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
		
	ImGui::EndChild();
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
