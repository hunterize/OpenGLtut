#pragma once

#include <glm.hpp>
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>
#include "CShader.h"
#include "CTexture.h"
#include "CMesh.h"

#include <vector>
#include <string>
#include <iostream>

class CModel
{
public:
	CModel(const std::string& path, bool gamma = false);
	~CModel();

	void Render(const CShader& shader);

private:
	std::vector<CTexture> m_LoadedTextures;
	std::vector<CMesh> m_Meshes;
	std::string m_Directory;
	bool m_GammaCorrection;

	void LoadModel(const std::string& path);
	void ProcessNode(aiNode* pNode, const aiScene* pScene);
	void ProcessMesh(aiMesh* pMesh, const aiScene* pScene);
	void GetTextures(aiMaterial* pMaterial, aiTextureType type, std::vector<CTexture>& textures);

};

