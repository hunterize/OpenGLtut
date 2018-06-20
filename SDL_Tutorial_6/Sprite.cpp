#include "Sprite.h"



Sprite::Sprite()
{
	m_uiVboID = 0;
}


Sprite::~Sprite()
{
	if (m_uiVboID == 0)
	{
		glDeleteBuffers(1, &m_uiVboID);
	}
}

void Sprite::init(float x, float y, float width, float height)
{
	m_iX = x;
	m_iY = y;
	m_iWidth = width;
	m_iHeight = height;

	if (m_uiVboID == 0)
	{
		glGenBuffers(1, &m_uiVboID);

	}
	
	//two triagle, every triangle has three vertices, every vertex has two data in 2D coordinate system
	float vertexData[12];

	//first triangle
	vertexData[0] = x + width;
	vertexData[1] = y + height;

	vertexData[2] = x;
	vertexData[3] = y + height;

	vertexData[4] = x;
	vertexData[5] = y;


	//second triangle
	vertexData[6] = x;
	vertexData[7] = y;

	vertexData[8] = x + width;
	vertexData[9] = y;

	vertexData[10] = x + width;
	vertexData[11] = y + height;

	glBindBuffer(GL_ARRAY_BUFFER, m_uiVboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);


	//unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);



}

void Sprite::draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_uiVboID);


	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);


	glDisableVertexAttribArray(0);


	//unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}



