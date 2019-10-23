#pragma once

#include <vector>


#include "CVertex.h"
#include "CTexture.h"
#include "CShader.h"


class CMesh
{
public:
	CMesh(const std::vector<CVertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<CTexture>& textures);
	~CMesh();

	void Render(const CShader& shader);
	void RenderInstance(const CShader& shader);
	void SetInstanceNumber(unsigned int num);

	std::vector<CVertex> m_Vertices;
	std::vector<unsigned int> m_Indices;
	std::vector<CTexture> m_Textures;
	unsigned int m_VAO;

private:

	unsigned int m_VBO;
	unsigned int m_EBO;
	unsigned int m_instanceNum;

	void Init();

};

