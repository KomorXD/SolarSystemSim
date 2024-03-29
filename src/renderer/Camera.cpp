#include "Camera.hpp"
#include "../Event.hpp"
#include "../Logger.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

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
	if (ev.Type == Event::WindowResized)
	{
		SetViewportSize({ (float)ev.Size.Width, (float)ev.Size.Height });

		return;
	}

	if (ev.Type == Event::MouseWheelScrolled)
	{
		m_Position += GetForwardDirection() * ev.MouseWheel.OffsetY * 2.0f;

		return;
	}
}

void Camera::OnUpdate(float ts)
{
	CheckForMouseMovement(ts);

	if (m_ControlType == CameraControlType::WorldControl)
	{
		CheckForMoveInput(ts);
	}
}

void Camera::Move(const glm::vec3& offset)
{
	m_Position += offset;
}

void Camera::MovePitch(float offset)
{
	m_Pitch += offset;
	m_Pitch = glm::max(glm::min(m_Pitch, 90.0f), -90.0f);
}

void Camera::MoveYaw(float offset)
{
	m_Yaw += offset;
}

void Camera::SetViewportSize(const glm::vec2& viewportSize)
{
	m_ViewportSize = viewportSize;
}

void Camera::SetPosition(const glm::vec3& position)
{
	m_Position = position;
}

void Camera::SetPitch(float pitch)
{
	m_Pitch = pitch;
}

void Camera::SetYaw(float yaw)
{
	m_Yaw = yaw;
}

void Camera::SetCameraControlType(CameraControlType controlType)
{
	m_ControlType = controlType;
}

void Camera::LookAt(const glm::vec3& point)
{
	glm::vec3 direction = glm::normalize(point - m_Position);

	m_Yaw   = glm::degrees(std::atan2f(direction.x, direction.z));
	m_Pitch = glm::degrees(std::asinf(-direction.y));
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
	return glm::quat(glm::vec3(glm::radians(-m_Pitch), glm::radians(-m_Yaw), 0.0f));
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

void Camera::CheckForMoveInput(float ts)
{
	glm::vec3 moveVec(0.0f);

	if (Input::IsKeyPressed(Key::A))
	{
		moveVec -= GetRightDirection();
	}
	else if (Input::IsKeyPressed(Key::D))
	{
		moveVec += GetRightDirection();
	}

	if (Input::IsKeyPressed(Key::W))
	{
		moveVec += GetForwardDirection();
	}
	else if (Input::IsKeyPressed(Key::S))
	{
		moveVec -= GetForwardDirection();
	}

	if (Input::IsKeyPressed(Key::Space))
	{
		moveVec += glm::vec3(0.0f, 1.0f, 0.0f);
	}
	else if (Input::IsKeyPressed(Key::LeftShift))
	{
		moveVec -= glm::vec3(0.0f, 1.0f, 0.0f);
	}

	if (glm::length(moveVec) != 0.0f)
	{
		m_Position += glm::normalize(moveVec) * 15.0f * ts;
	}
}

void Camera::CheckForMouseMovement(float ts)
{
	glm::vec2 mousePos = Input::GetMousePosition();
	glm::vec2 delta = mousePos - m_InitialMousePos;

	m_InitialMousePos = mousePos;

	float deltaLen = glm::length(delta);

	if (deltaLen == 0.0f || deltaLen > 100.0f)
	{
		return;
	}

	if (Input::IsMouseButtonPressed(MouseButton::Right))
	{
		Input::DisableCursor();

		m_Yaw   += delta.x * 0.1f;
		m_Pitch -= delta.y * 0.1f;

		m_Pitch = glm::max(glm::min(m_Pitch, 90.0f), -90.0f);

		return;
	}

	if (Input::IsMouseButtonPressed(MouseButton::Middle))
	// if (Input::IsKeyPressed(Key::LeftControl))
	{
		Input::DisableCursor();

		m_Position -= GetRightDirection() * delta.x * 0.05f;
		m_Position -= GetUpDirection()	  * delta.y * 0.05f;

		return;
	}

	if (m_ControlType == CameraControlType::EditorControl && !Input::IsMouseButtonPressed(MouseButton::Middle))
	{
		Input::ShowCursor();
	}
}
