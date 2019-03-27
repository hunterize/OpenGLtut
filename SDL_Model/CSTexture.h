#pragma once
#include <glew.h>
#include <string>
#include "stb_image.h"
#include <iostream>

struct GLTexture
{
	GLuint ID;
	int width;
	int height;
};

class CSTexture
{
public:
	GLTexture static LoadImage(const std::string& fileName);
};

