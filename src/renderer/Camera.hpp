#pragma once

#include <glm/glm.hpp>

struct Event;

enum class CameraControlType
{
	EditorControl,
	WorldControl
};

class Camera
{
public:
	Camera();
	Camera(float fov, float aspectRatio, float nearClip, float farClip);

	void OnEvent(Event& ev);
	void OnUpdate(float ts);

	void Move(const glm::vec3& offset);
	void MovePitch(float offset);
	void MoveYaw(float offset);

	void SetViewportSize(const glm::vec2& viewportSize);
	void SetPosition(const glm::vec3& position);
	void SetCameraControlType(CameraControlType controlType);

	glm::vec3 GetUpDirection()		const;
	glm::vec3 GetRightDirection()	const;
	glm::vec3 GetForwardDirection() const;
	glm::quat GetOrientation()		const;

	inline const glm::mat4& GetProjection() const { return m_Projection; }
	inline const glm::mat4& GetViewMatrix() const { return m_View; }
	inline glm::mat4 GetViewProjection()	const { return m_Projection * m_View; }
	inline glm::vec3 GetPosition()			const { return m_Position; }
	inline float GetPitch()					const { return m_Pitch; }
	inline float GetYaw()					const { return m_Yaw; }

private:
	void UpdateProjection();
	void UpdateView();

	void CheckForMoveInput(float ts);
	void CheckForMouseMovement(float ts);

	CameraControlType m_ControlType = CameraControlType::EditorControl;

	float m_FOV			= 45.0f;
	float m_AspectRatio = 16.0f / 9.0f;
	float m_NearClip	= 0.1f;
	float m_FarClip		= 100.0f;

	glm::mat4 m_Projection = glm::mat4(1.0f);
	glm::mat4 m_View	   = glm::mat4(1.0f);

	glm::vec3 m_Position		= { 0.0f, 0.0f, 0.0f };
	glm::vec2 m_InitialMousePos = { 0.0f, 0.0f };

	float m_Pitch = 0.0f;
	float m_Yaw	  = 0.0f;

	glm::vec2 m_ViewportSize = { 1280.0f, 720.0f };
};