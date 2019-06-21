
#define STB_IMAGE_IMPLEMENTATION 
#include "CSTexture.h"



GLTexture CSTexture::LoadImage(const std::string& fileName)
{
	GLTexture texture = {};
	texture.ID = -1;
	int channel;

	glGenTextures(1, &texture.ID);

	//flip the texture
	stbi_set_flip_vertically_on_load(true);

	unsigned char *imageData = NULL;
	imageData = stbi_load(fileName.c_str(), &texture.width, &texture.height, &channel, 0);

	if(imageData)
	{
		GLenum format;
		if (channel == 1)
		{
			format = GL_RED;
		}
		else if (channel == 3)
		{
			format = GL_RGB;
		}
		else if (channel == 4)
		{
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, texture.ID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0, format, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

	}
	else
	{
		std::cout << "Failed to load image: " << fileName << std::endl;
	}

	stbi_image_free(imageData);

	return texture;
}