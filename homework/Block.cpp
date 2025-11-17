#include "Block.h"
#include "headHeader.h"

Block::Block(float, float, float, float)
{
}

void Block::update()
{
}

void Block::draw(GLuint VBO)
{
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), vertex.data(), GL_STATIC_DRAW);
}
