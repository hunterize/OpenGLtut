#pragma once

#include <string>
#include <glm.hpp>
#include "stb_image.h"
#include <glew.h>
#include <iostream>


/*
	m_Type: string type of the texture
	        {"texture_diffuse", "texture_specular"}

*/

struct CTexture
{
	unsigned int m_ID;
	std::string m_Type;
	std::string m_FileName;

	static unsigned int LoadImage(const std::string& fileName);
};