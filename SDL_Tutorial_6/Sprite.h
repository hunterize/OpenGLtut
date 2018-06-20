#pragma once
#include <GLEW\glew.h>

class Sprite
{
public:
	Sprite();
	~Sprite();

	void init(float x, float y, float width, float height);
	void draw();

private:
	float m_iX;
	float m_iY;
	float m_iWidth;
	float m_iHeight;
	//vertex buffer object ID
	//unsigned int m_uiVboID;
	GLuint m_uiVboID;

};

