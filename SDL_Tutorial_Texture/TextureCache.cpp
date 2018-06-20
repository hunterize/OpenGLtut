#include "TextureCache.h"
#include "ImageLoader.h"
#include <utility>
#include <iostream>


TextureCache::TextureCache()
{
}


TextureCache::~TextureCache()
{
}


GLTexture TextureCache::GetTexture(std::string texturePath)
{
	std::map<std::string, GLTexture>::iterator mit = m_cTextureMap.find(texturePath);
	//auto mit = m_cTextureMap.find(texturePath);

	if (mit == m_cTextureMap.end())
	{
		GLTexture newTexture = ImageLoader::LoadPNG(texturePath);

		std::pair<std::string, GLTexture> newPair(texturePath, newTexture);

		m_cTextureMap.insert(newPair);
		//m_cTextureMap.insert(std::make_pair(texturePath, newTexture));

		std::cout << "Loaded Texture" << std::endl;

		return newTexture;	
	}

	std::cout << "Used Cached Texture" << std::endl;

	return mit->second;

}