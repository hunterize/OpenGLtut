#pragma once
#include "GLTexture.h"
#include <map>		
#include <string>



class TextureCache
{
public:
	TextureCache();
	~TextureCache();

	GLTexture GetTexture(std::string);

private:
	std::map<std::string, GLTexture> m_cTextureMap;
};

