#include "GeometryFactory.h"

namespace GeometryFactory
{
	//total vertex numbers is (nstack - 1) * (nslice + 1) + 2
	//the Geomery has (nstack - 1) rings, (nslice + 1) vertices on each ring and two pole vertices.
	void GGenerator::GetSphere(GLuint nstack, GLuint nslice, GMesh& mesh)
	{
		GLfloat radius = 0.5f;
		mesh.m_vertices.clear();
		mesh.m_indices.clear();

		////vertices
		GVertex top(0.0f, radius, 0.0f, 0.0f, 1.0f, 0.0f, 1.0, 0.0, 0.0, 0.0, 1.0);
		GVertex bottom(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0, 0.0);

		//top vertex of the sphere
		mesh.m_vertices.push_back(top);
		//vertices in the sphere
		GLfloat lat = PI / nstack;
		GLfloat longit = 2.0f * PI / nslice;

		for (int i = 1; i < nstack; i++)
		{
			for (int j = 0; j < nslice; j++)
			{
				GVertex vertex;
				vertex.m_position.x = radius * glm::sin(i * lat) * glm::cos(j * longit);
				vertex.m_position.y = radius * glm::cos(i * lat);
				vertex.m_position.z = -radius * glm::sin(i * lat) * glm::sin(j * longit);

				vertex.m_normal = glm::normalize(vertex.m_position);

				vertex.m_tangent.x = -glm::sin(j * longit);
				vertex.m_tangent.y = 0.0f;
				vertex.m_tangent.z = -glm::cos(j * longit);

				vertex.m_texCoord.x = (GLfloat)j / nslice;
				vertex.m_texCoord.y = 1.0 - (GLfloat)i / nstack;

				mesh.m_vertices.push_back(vertex);
			}
			
			//add slight offset to the last vertex on a ring
			int j = nslice;
			GVertex vertex;
			GLfloat delta = j * longit + 0.01;

			vertex.m_position.x = radius * glm::sin(i * lat) * glm::cos(delta);
			vertex.m_position.y = radius * glm::cos(i * lat);
			vertex.m_position.z = -radius * glm::sin(i * lat) * glm::sin(delta);

			vertex.m_normal = glm::normalize(vertex.m_position);

			vertex.m_tangent.x = -glm::sin(delta);
			vertex.m_tangent.y = 0.0f;
			vertex.m_tangent.z = -glm::cos(delta);

			vertex.m_texCoord.x = (GLfloat)j / nslice;
			vertex.m_texCoord.y = 1.0 - (GLfloat)i / nstack;

			mesh.m_vertices.push_back(vertex);
		}
		//bottom vertex of the sphere
		mesh.m_vertices.push_back(bottom);

		////indices
		// top vertex and the fist ring
		for (GLuint i = 1; i <= nslice; i++)
		{
			mesh.m_indices.push_back(0);
			mesh.m_indices.push_back(i);
			mesh.m_indices.push_back(i + 1);
		}

		//starting index in the vertex array
		GLuint iStart = 1;
		//vertex count on a ring, the last one meets the first one to make a circle
		GLuint nVertices = nslice + 1;

		//loop each ring
		for (GLuint i = 0; i < nstack - 2; i++)
		{
			//loop every vertex on a ring
			for (GLuint j = 0; j < nslice; j++)
			{
				//4 vertices indice for 2 triangles
				mesh.m_indices.push_back(iStart + i * nVertices + j);
				mesh.m_indices.push_back(iStart + (i + 1) * nVertices + j);
				mesh.m_indices.push_back(iStart + (i + 1) * nVertices + j + 1);

				mesh.m_indices.push_back(iStart + (i + 1) * nVertices + j + 1);
				mesh.m_indices.push_back(iStart + i * nVertices + j + 1);
				mesh.m_indices.push_back(iStart + i * nVertices + j);
			}
		}

		//index of the final vertex
		GLuint iBottom = mesh.m_vertices.size() - 1;
		//starting index for the last ring
		GLuint iLastRing = iBottom - nVertices;

		for (GLuint i = 0; i < nslice; i++)
		{
			mesh.m_indices.push_back(iBottom);
			mesh.m_indices.push_back(iLastRing + i + 1);
			mesh.m_indices.push_back(iLastRing + i);
		}

	}

}