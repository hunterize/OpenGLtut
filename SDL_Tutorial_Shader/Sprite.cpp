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
	Vertex vertexData[6];

	//first triangle
	vertexData[0].SetPosition(x + width, y + height);
	vertexData[0].SetUV(1.0f, 1.0f);

	vertexData[1].SetPosition(x, y + height);
	vertexData[1].SetUV(0.0f, 1.0f);

	vertexData[2].SetPosition(x, y);
	vertexData[2].SetUV(0.0f, 0.0f);

	//second triangle
	vertexData[3].SetPosition(x, y);
	vertexData[3].SetUV(0.0f, 0.0f);

	vertexData[4].SetPosition(x + width, y);
	vertexData[4].SetUV(1.0f, 0.0f);

	vertexData[5].SetPosition(x + width, y + height);
	vertexData[5].SetUV(1.0f, 1.0f);

	//all the vertex has same color
	for (int i = 0; i < 6; i++)
	{
		vertexData[i].SetColor(255, 0, 255, 255);
	}

	//change some of the color
	vertexData[1].SetColor(0, 0, 255, 255);

	vertexData[4].SetColor(0, 255, 0, 255);


	glBindBuffer(GL_ARRAY_BUFFER, m_uiVboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);


	//unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);



}

void Sprite::draw()
{
	//bind the buffer object
	glBindBuffer(GL_ARRAY_BUFFER, m_uiVboID);


	//tell opengl that we want to use the first
	//attribute array. we only need one array right now
	//since we are only using position
	//glEnableVertexAttribArray(0);


	//this is the position attribute pointer
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	//this is the color attribute pointer, color elements are represented by one byte data 0-255, it needs to be normalized
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color));

	//this is the uv attribute pointer
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	//draw the 6 vertices to the screen
	glDrawArrays(GL_TRIANGLES, 0, 6);


	//disable the vertex attrib array. this is not optional
	//glDisableVertexAttribArray(0);


	//unbind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}



