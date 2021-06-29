#pragma once

#include <glm.hpp>

struct CVertex
{
	glm::vec3 m_Position;
	glm::vec3 m_Normal;
	glm::vec2 m_TexCoords;
	glm::vec3 m_Tangent;
	glm::vec3 m_Bitangent;
};
