#include "Camera.hpp"
#include "../Event.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

Camera::Camera()
{
	UpdateProjection();
	UpdateView();
}

Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
	: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
{
	UpdateProjection();
	UpdateView();
}

void Camera::OnEvent(Event& ev)
{
}

void Camera::OnUpdate(float ts)
{
	glm::vec3 moveVec(0.0f);

	if (Input::IsKeyPressed(Key::A))
	{
		moveVec.x = -1.0f;
	}
	else if (Input::IsKeyPressed(Key::D))
	{
		moveVec.x = 1.0f;
	}

	if (Input::IsKeyPressed(Key::W))
	{
		moveVec.z = -1.0f;
	}
	else if (Input::IsKeyPressed(Key::S))
	{
		moveVec.z = 1.0f;
	}

	if (Input::IsKeyPressed(Key::Space))
	{
		moveVec.y = 1.0f;
	}
	else if (Input::IsKeyPressed(Key::LeftShift))
	{
		moveVec.y = -1.0f;
	}

	if (glm::length(moveVec) != 0.0f)
	{
		m_Position += glm::normalize(moveVec) * ts;
		UpdateView();
	}
}

void Camera::SetViewportSize(const glm::vec2& viewportSize)
{
	m_ViewportSize = viewportSize;
	UpdateProjection();
}

void Camera::SetPosition(const glm::vec3& position)
{
	m_Position = position;
	UpdateView();
}

glm::vec3 Camera::GetUpDirection() const
{
	return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 Camera::GetRightDirection() const
{
	return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 Camera::GetForwardDirection() const
{
	return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::quat Camera::GetOrientation() const
{
	return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
}

void Camera::UpdateProjection()
{
	m_AspectRatio = m_ViewportSize.x / m_ViewportSize.y;
	m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
}

void Camera::UpdateView()
{
	glm::quat orientation = GetOrientation();

	m_View = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
	m_View = glm::inverse(m_View);
}
