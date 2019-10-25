#include "CCamera3D.h"



CCamera3D::CCamera3D(
	float screenWidth, float screenHeight,
	glm::vec3 position,
	glm::vec3 front)
{
	m_cPosition = position;
	m_cFront = front;
	m_cUp = glm::vec3(0.0f, 1.0f, 0.0f);
	m_fInitSpeed = 8.0f;
	m_fTackleSpeed = 1.0f;
	m_fSpeed = 8.0f;
	m_fSensitivity = 2.0f;

	m_cWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	m_fPitch = 0.0f;
	m_fYaw = -90.0f;
	m_fFov = 45.0f;

	m_fScreenWidth = screenWidth;
	m_fScreenHeight = screenHeight;

	UpdateCameraVector();
}

CCamera3D::CCamera3D(
	float screenWidth, float screenHeight,
	bool isInit,
	glm::vec3 position,
	glm::vec3 front)
{
	m_cPosition = position;
	m_cFront = front;
	m_cUp = glm::vec3(0.0f, 1.0f, 0.0f);
	m_fInitSpeed = 8.0f;
	m_fTackleSpeed = 1.0f;
	m_fSpeed = 8.0f;
	m_fSensitivity = 2.0f;

	m_cWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	m_fPitch = 0.0f;
	m_fFov = 45.0f;
	m_fScreenWidth = screenWidth;
	m_fScreenHeight = screenHeight;
	
	if (isInit)
	{
		m_fYaw = -90.0f;
		UpdateCameraVector();
	}
	else
	{
		m_cFront = glm::normalize(m_cFront);
		float yaw = m_cFront.x == 0 ? 0.0f : glm::atan(m_cFront.z, m_cFront.x);
		m_fYaw = (yaw + 3.14f) * 360.0f / (2 * 3.14f) - 180.0f;
		float pitch = glm::atan(m_cFront.y, glm::sqrt(m_cFront.x * m_cFront.x + m_cFront.z * m_cFront.z));
		m_fPitch = (pitch + 3.14f) * 360.0f / (2 * 3.14f) - 180.0f;
		m_cRight = glm::normalize(glm::cross(m_cFront, m_cWorldUp));
		m_cUp = glm::normalize(glm::cross(m_cRight, m_cFront));
	}
}

CCamera3D::~CCamera3D()
{
}

void CCamera3D::SetSpeed(float speed)
{
	m_fInitSpeed = speed;
	m_fTackleSpeed = speed / 10.0f;
	m_fSpeed = m_fInitSpeed;
}

glm::mat4 CCamera3D::GetCameraMatrix()
{
	return glm::lookAt(m_cPosition, m_cPosition + m_cFront, m_cUp);
}

glm::mat4 CCamera3D::GetReverseCameraMatrix()
{
	return glm::lookAt(m_cPosition, m_cPosition - m_cFront, m_cUp);
}

void CCamera3D::Update(const CInputManager& input, float timeSpan)
{
	if (input.IsKeyDown(SDLK_w))
	{
		m_cPosition += m_cFront * m_fSpeed * timeSpan;
	}
	if (input.IsKeyDown(SDLK_s))
	{
		m_cPosition -= m_cFront * m_fSpeed * timeSpan;
	}
	if (input.IsKeyDown(SDLK_a))
	{
		m_cPosition -= m_cRight * m_fSpeed * timeSpan;
	}
	if (input.IsKeyDown(SDLK_d))
	{
		m_cPosition += m_cRight * m_fSpeed * timeSpan;
	}
	if (input.IsKeyDown(SDLK_LCTRL))
	{
		m_fSpeed = m_fTackleSpeed;
	}
	else
	{
		m_fSpeed = m_fInitSpeed;
	}

	if(input.GetMouseCoord() != glm::vec2(-1.0f))
	{
		float offsetX = input.GetMouseCoord().x - m_fScreenWidth / 2;
		float offsetY = input.GetMouseCoord().y - m_fScreenHeight / 2;
	
		m_fYaw += offsetX * m_fSensitivity * timeSpan / 2.0f;
		m_fPitch -= offsetY * m_fSensitivity * timeSpan / 2.0f;

		m_fPitch = m_fPitch > 89.0f ? 89.0 : m_fPitch;
		m_fPitch = m_fPitch < -89.0f ? -89.0f : m_fPitch;
		UpdateCameraVector();		
	}

}

void CCamera3D::UpdateCameraVector()
{
	glm::vec3 direction;

	direction.x = cos(glm::radians(m_fYaw)) * cos(glm::radians(m_fPitch));
	direction.y = sin(glm::radians(m_fPitch));
	direction.z = sin(glm::radians(m_fYaw)) * cos(glm::radians(m_fPitch));

	m_cFront = glm::normalize(direction);

	m_cRight = glm::normalize(glm::cross(m_cFront, m_cWorldUp));
	m_cUp = glm::normalize(glm::cross(m_cRight, m_cFront));
}
