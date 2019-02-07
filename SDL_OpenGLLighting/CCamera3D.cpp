#include "CCamera3D.h"



CCamera3D::CCamera3D(
	float screenWidth, float screenHeight,
	glm::vec3 position,
	glm::vec3 front)
{
	m_cPosition = position;
	m_cFront = front;
	m_cUp = glm::vec3(0.0f, 1.0f, 0.0f);
	m_fSpeed = 10.0f;
	m_fSensitivity = 8.0f;

	m_cWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	m_fPitch = 0.0f;
	m_fYaw = -90.0f;
	m_fFov = 45.0f;

	m_fScreenWidth = screenWidth;
	m_fScreenHeight = screenHeight;

	UpdateCameraVector();
}


CCamera3D::~CCamera3D()
{
}

glm::mat4 CCamera3D::GetCameraMatrix()
{
	return glm::lookAt(m_cPosition, m_cPosition + m_cFront, m_cUp);
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
