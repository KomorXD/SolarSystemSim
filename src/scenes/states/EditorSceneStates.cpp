#include "EditorSceneStates.hpp"
#include "../../renderer/Renderer.hpp"

NewSphereState::NewSphereState(EditorScene& scene)
	: m_ParentScene(scene)
{
}

NewSphereState::~NewSphereState()
{
}

void NewSphereState::OnEvent(Event& ev)
{
}

void NewSphereState::OnUpdate(float ts)
{
}

void NewSphereState::OnRender()
{
}
