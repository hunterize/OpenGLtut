#include "ResourceManager.h"

TextureCache ResourceManager::m_cTextureCache;

GLTexture ResourceManager::GetTexture(std::string texturePath)
{
	return m_cTextureCache.GetTexture(texturePath);
}