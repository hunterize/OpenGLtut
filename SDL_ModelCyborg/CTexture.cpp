#include "CTexture.h"


unsigned int CTexture::LoadImage(const std::string& fileName)
{
	unsigned int ID;
	int width, height, channel;

	glGenTextures(1, &ID);

	//flip the texture
	//stbi_set_flip_vertically_on_load(true);

	unsigned char *imageData = NULL;
	imageData = stbi_load(fileName.c_str(), &width, &height, &channel, 0);

	if (imageData)
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

		glBindTexture(GL_TEXTURE_2D, ID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

	}
	else
	{
		std::cout << "Failed to load image: " << fileName << std::endl;
	}

	stbi_image_free(imageData);

	return ID;
}



