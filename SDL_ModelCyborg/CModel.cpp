#include "CModel.h"



CModel::CModel(const std::string& path, bool gamma) : m_GammaCorrection(gamma)
{
	LoadModel(path);
}


CModel::~CModel()
{
}

void CModel::Render(const CShader& shader)
{
	for (auto& mesh : m_Meshes)
	{
		mesh.Render(shader);
	}
}

void CModel::LoadModel(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	
	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
	{
		std::cout << "Assimp load file failed: " << importer.GetErrorString() << std::endl;
		return;
	}

	m_Directory = path.substr(0, path.find_last_of('/'));
	ProcessNode(pScene->mRootNode, pScene);
}

void CModel::ProcessNode(aiNode* pNode, const aiScene* pScene)
{
	for (int i = 0; i < pNode->mNumMeshes; i++)
	{
		aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[i]];
		ProcessMesh(pMesh, pScene);
	}

	for (int i = 0; i < pNode->mNumChildren; i++)
	{
		ProcessNode(pNode->mChildren[i], pScene);
	}
}

void CModel::ProcessMesh(aiMesh* pMesh, const aiScene* pScene)
{
	std::vector<CVertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<CTexture> textures;

	//get vertices of the mesh
	for (int i = 0; i < pMesh->mNumVertices; i++)
	{
		CVertex vertex;
		//get vertex position
		vertex.m_Position.x = pMesh->mVertices[i].x;
		vertex.m_Position.y = pMesh->mVertices[i].y;
		vertex.m_Position.z = pMesh->mVertices[i].z;
		//get vertex normal
		vertex.m_Normal.x = pMesh->mNormals[i].x;
		vertex.m_Normal.y = pMesh->mNormals[i].y;
		vertex.m_Normal.z = pMesh->mNormals[i].z;
		//get vertex texture UV, only use the first coordinate
		if (pMesh->mTextureCoords[0])
		{
			vertex.m_TexCoords.x = pMesh->mTextureCoords[0][i].x;
			vertex.m_TexCoords.y = pMesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.m_TexCoords.x = 0.0f;
			vertex.m_TexCoords.y = 0.0f;
		}
		//get vertex tangent vector
		vertex.m_Tangent.x = pMesh->mTangents[i].x;
		vertex.m_Tangent.y = pMesh->mTangents[i].y;
		vertex.m_Tangent.z = pMesh->mTangents[i].z;
		//get vertex bitangent vector
		vertex.m_Bitangent.x = pMesh->mBitangents[i].x;
		vertex.m_Bitangent.y = pMesh->mBitangents[i].y;
		vertex.m_Bitangent.z = pMesh->mBitangents[i].z;

		vertices.push_back(vertex);

	}

	//get vertex indices
	for (int i = 0; i < pMesh->mNumFaces; i++)
	{
		aiFace face = pMesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	//get materials
	aiMaterial* material = pScene->mMaterials[pMesh->mMaterialIndex];
	GetTextures(material, aiTextureType_AMBIENT, textures);
	GetTextures(material, aiTextureType_DIFFUSE, textures);
	GetTextures(material, aiTextureType_SPECULAR, textures);
	GetTextures(material, aiTextureType_HEIGHT, textures);

	m_Meshes.push_back(CMesh(vertices, indices, textures));

}

void CModel::GetTextures(aiMaterial* pMaterial, aiTextureType type, std::vector<CTexture>& textures)
{
	//std::vector<CTexture> textures;

	for (int i = 0; i < pMaterial->GetTextureCount(type); i++)
	{
		aiString fileName;
		//get texture file name
		pMaterial->GetTexture(type, i, &fileName);

		bool isSkip = false;

		for (int j = 0; j < m_LoadedTextures.size(); j++)
		{
			if (std::strcmp(m_LoadedTextures[j].m_FileName.c_str(), fileName.C_Str()) == 0)
			{
				textures.push_back(m_LoadedTextures[j]);
				isSkip = true;
				break;
			}
		}

		if (!isSkip)
		{
			CTexture texture;
			std::string textureFile = m_Directory + '/' + std::string(fileName.C_Str());
			texture.m_ID = CTexture::LoadImage(textureFile.c_str());
			texture.m_FileName = std::string(fileName.C_Str());
			switch (type)
			{
			case aiTextureType_AMBIENT:
				texture.m_Type = "texture_ambient";
				break;
			case aiTextureType_DIFFUSE:
				texture.m_Type = "texture_diffuse";
				break;
			case aiTextureType_SPECULAR:
				texture.m_Type = "texture_specular";
				break;
			case aiTextureType_HEIGHT:
				texture.m_Type = "texture_normal";
				break;
			default:
				texture.m_Type = "NA";
			}

			textures.push_back(texture);
			m_LoadedTextures.push_back(texture);
		}

	}
}