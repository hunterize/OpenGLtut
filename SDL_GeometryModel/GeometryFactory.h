#pragma once

#include <SDL.h>
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>

namespace GeometryFactory
{
	const GLfloat PI = 3.1415f;
	struct GVertex
	{
		GVertex() : m_position(0.0), m_normal(0.0), m_tangent(0.0), m_texCoord(0.0) {};
		GVertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& tangent, const glm::vec2& texCoord)
			: m_position(position), m_normal(normal), m_tangent(tangent), m_texCoord(texCoord) {};
		GVertex(
			GLfloat px, GLfloat py, GLfloat pz,
			GLfloat nx, GLfloat ny, GLfloat nz,
			GLfloat tx, GLfloat ty, GLfloat tz,
			GLfloat u, GLfloat v)
			: m_position(px, py, pz), m_normal(nx, ny, nz), m_tangent(tx, ty, tz), m_texCoord(u, v) {};

		glm::vec3 m_position;
		glm::vec3 m_normal;
		glm::vec3 m_tangent;
		glm::vec2 m_texCoord;
	};

	struct GMesh
	{
		std::vector<GVertex> m_vertices;
		std::vector<GLuint> m_indices;
	};

	class GGenerator
	{
	public:
		static void GetSphere(GLuint nstack, GLuint nslice, GMesh& mesh);
	};

}