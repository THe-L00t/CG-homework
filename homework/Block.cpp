#include "Block.h"

Block::Block(float ix, float iz, float xS, float zS)
	:x{ix},z{iz},xSize{xS},zSize{zS}
{
	vertex.reserve(90);
	createVertex();
}

void Block::update(float deltaT)
{
	height += hDir * deltaT * speed;
	if (height > maxH) {
		height -= hDir * deltaT * speed;
		hDir = -1.f;
	}
	else if (height < minH) {
		height += hDir * deltaT * speed;
		hDir = 1.f;
	}
	vertex.clear();
	createVertex();
}

void Block::draw(GLuint VBO)
{
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), vertex.data(), GL_STATIC_DRAW);
}

void Block::createVertex()
{
	//뒷면 start
	vertex.push_back(x - xSize / 2);
	vertex.push_back(0);
	vertex.push_back(z - xSize / 2);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(height);
	vertex.push_back(z - xSize / 2);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(0);
	vertex.push_back(z - xSize / 2);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(height);
	vertex.push_back(z - xSize / 2);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(height);
	vertex.push_back(z - xSize / 2);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(0);
	vertex.push_back(z - xSize / 2);
	// 뒷면 end

	//오른쪽 start
	vertex.push_back(x + xSize / 2);
	vertex.push_back(0);
	vertex.push_back(z - xSize / 2);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(height);
	vertex.push_back(z - xSize / 2);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(0);
	vertex.push_back(z + xSize / 2);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(height);
	vertex.push_back(z - xSize / 2);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(height);
	vertex.push_back(z + xSize / 2);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(0);
	vertex.push_back(z + xSize / 2);
	// 오른쪽 end

	//왼쪽 start
	vertex.push_back(x - xSize / 2);
	vertex.push_back(0);
	vertex.push_back(z - xSize / 2);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(height);
	vertex.push_back(z - xSize / 2);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(0);
	vertex.push_back(z + xSize / 2);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(height);
	vertex.push_back(z - xSize / 2);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(height);
	vertex.push_back(z + xSize / 2);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(0);
	vertex.push_back(z + xSize / 2);
	// 왼쪽 end

	//앞면 start
	vertex.push_back(x - xSize / 2);
	vertex.push_back(0);
	vertex.push_back(z + xSize / 2);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(height);
	vertex.push_back(z + xSize / 2);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(0);
	vertex.push_back(z + xSize / 2);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(height);
	vertex.push_back(z + xSize / 2);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(height);
	vertex.push_back(z + xSize / 2);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(0);
	vertex.push_back(z + xSize / 2);
	// 앞면 end

	//위쪽 start
	vertex.push_back(x - xSize / 2);
	vertex.push_back(height);
	vertex.push_back(z - xSize / 2);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(height);
	vertex.push_back(z + xSize / 2);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(height);
	vertex.push_back(z - xSize / 2);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(height);
	vertex.push_back(z - xSize / 2);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(height);
	vertex.push_back(z + xSize / 2);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(height);
	vertex.push_back(z + xSize / 2);
	// 위쪽 end
}
