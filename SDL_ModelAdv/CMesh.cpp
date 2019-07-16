#include "CMesh.h"



CMesh::CMesh(const std::vector<CVertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<CTexture>& textures)
{
	this->m_Vertices = vertices;
	this->m_Indices = indices;
	this->m_Textures = textures;

	this->m_EBO = 0;
	this->m_VAO = 0;
	this->m_VBO = 0;

	Init();
}


CMesh::~CMesh()
{
}


void CMesh::Render(const CShader& shader)
{
	int diffuseNum = 1;
	int specularNum = 1;

	//bind texture to shader
	for (int i = 0; i < m_Textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		std::string name = m_Textures[i].m_Type;
		std::string number;
		if (name == "texture_diffuse")
		{
			number = std::to_string(diffuseNum++);
		}
		else if (name == "texture_specular")
		{
			number = std::to_string(specularNum++);
		}

		shader.SetUniformInt(name + number, i);
		glBindTexture(GL_TEXTURE_2D, m_Textures[i].m_ID);
	}

	//draw
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//glActiveTexture(GL_TEXTURE0);
}

void CMesh::Init()
{
	//initialize vao, vbo, ebo
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);


	// vertex array
	glBindVertexArray(m_VAO);
	//bind vertice data
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(CVertex), &m_Vertices[0], GL_STATIC_DRAW);
	//bind vertice indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

	//set attributes
	//location 0 for position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CVertex), (void*)0);
	//location 1 for normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CVertex), (void*)offsetof(CVertex, m_Normal));
	//location 2 for texture UV
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(CVertex), (void*)offsetof(CVertex, m_TexCoords));

	glBindVertexArray(0);


}