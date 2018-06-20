#pragma once
#include "TextureCache.h"
#include <string>

class ResourceManager
{
public:
	static GLTexture GetTexture(std::string texturePath);
	

private:
	static TextureCache m_cTextureCache;

};

