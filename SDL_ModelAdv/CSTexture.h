#pragma once
#include <glew.h>
#include <string>
#include "stb_image.h"
#include <iostream>

#include <vector>

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

	//folder contains files for cube maps, names should be 
	//"back", "front", "top", "bottom", "left", "right"
	//
	GLuint static LoadCubeMaps(const std::vector<std::string>& faces);

	//flexible way to define the function to load cube maps
	GLuint static LoadCubeMaps(const std::string& front, const std::string& back,
		const std::string& top, const std::string& bottom,
		const std::string& left, const std::string& right);

protected:
	void static LoadCubeFace(GLenum format, const std::string& face);
};

