#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <SDL.h>

#include "CInputManager.h"

class CCamera3D
{
public:
	CCamera3D(float screenWidth, float screenHeight,
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
		glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f));
	CCamera3D(float screenWidth, float screenHeight,
		bool isInit,
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f));
	~CCamera3D();

	glm::mat4 GetCameraMatrix();
	glm::mat4 GetReverseCameraMatrix();

	void Update(const CInputManager& input, float timeSpan);

	void SetSpeed(float speed) { m_fSpeed = speed; }
	void SetSensitivity(float se) { m_fSensitivity = se; }
	glm::vec3 GetPosition() const { return m_cPosition; } 
	glm::vec3 GetFront() const { return m_cFront; }

private:
	void UpdateCameraVector();

	glm::vec3 m_cPosition;
	glm::vec3 m_cFront;
	glm::vec3 m_cUp;
	glm::vec3 m_cRight;
	glm::vec3 m_cWorldUp;

	float m_fSpeed;
	float m_fSensitivity;
	float m_fFov;
	float m_fYaw;
	float m_fPitch;

	float m_fScreenWidth;
	float m_fScreenHeight;
};

