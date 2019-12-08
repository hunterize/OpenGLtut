
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

void CSTexture::LoadCubeFace(GLenum textureTarget, const std::string& face)
{
	int width, height, channel;
	unsigned char *imageData = NULL;

	stbi_set_flip_vertically_on_load(false);
	imageData = stbi_load(face.c_str(), &width, &height, &channel, 0);

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

		glTexImage2D(textureTarget, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);

	}
	else
	{
		std::cout << "Failed to load image: " << face << std::endl;
	}

	stbi_image_free(imageData);
}

GLuint CSTexture::LoadCubeMaps(const std::string& front, const std::string& back,
	const std::string& top, const std::string& bottom,
	const std::string& left, const std::string& right)
{
	GLuint id = -1;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	LoadCubeFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X, right);
	LoadCubeFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left);
	LoadCubeFace(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, top);
	LoadCubeFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottom);
	LoadCubeFace(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, back);
	LoadCubeFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, front);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return id;
}

GLuint CSTexture::LoadCubeMaps(const std::vector<std::string>& faces)
{
	GLTexture texture = {};
	texture.ID = -1;
	int channel;

	glGenTextures(1, &texture.ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture.ID);


	//flip the texture
	stbi_set_flip_vertically_on_load(false);

	for (int i = 0; i < faces.size(); i++)
	{
		unsigned char *imageData = NULL;
		imageData = stbi_load(faces[i].c_str(), &texture.width, &texture.height, &channel, 0);

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

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, texture.width, texture.height, 0, format, GL_UNSIGNED_BYTE, imageData);
			//glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load image: " << faces[i] << std::endl;
		}

		stbi_image_free(imageData);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return texture.ID;
}