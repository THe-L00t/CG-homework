#include "Block.h"

Block::Block(float ix, float iz, float xS, float zS)
	:x{ix},z{iz},xSize{xS},zSize{zS}
{
	vertex.reserve(90);

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
	//µÞ¸é start
	vertex[0] = x - xSize / 2;
	vertex[1] = 0;
	vertex[2] = z - xSize / 2;

	vertex[3] = x - xSize / 2;
	vertex[4] = height;
	vertex[5] = z - xSize / 2;

	vertex[6] = x + xSize / 2;
	vertex[7] = 0;
	vertex[8] = z - xSize / 2;

	vertex[9] = x - xSize / 2;
	vertex[10] = height;
	vertex[11] = z - xSize / 2;

	vertex[12] = x + xSize / 2;
	vertex[13] = height;
	vertex[14] = z - xSize / 2;

	vertex[15] = x + xSize / 2;
	vertex[16] = 0;
	vertex[17] = z - xSize / 2;
	// µÞ¸é end

	//¿À¸¥¸é start
	vertex[18] = x + xSize / 2;
	vertex[19] = 0;
	vertex[20] = z - xSize / 2;

	vertex[21] = x + xSize / 2;
	vertex[22] = height;
	vertex[23] = z - xSize / 2;

	vertex[24] = x + xSize / 2;
	vertex[25] = 0;
	vertex[26] = z + xSize / 2;

	vertex[27] = x + xSize / 2;
	vertex[28] = height;
	vertex[29] = z - xSize / 2;

	vertex[30] = x + xSize / 2;
	vertex[31] = height;
	vertex[32] = z + xSize / 2;

	vertex[33] = x + xSize / 2;
	vertex[34] = 0;
	vertex[35] = z + xSize / 2;
	// ¿À¸¥¸é end

	//¿Þ¸é start
	vertex[36] = x - xSize / 2;
	vertex[37] = 0;
	vertex[38] = z - xSize / 2;

	vertex[39] = x - xSize / 2;
	vertex[40] = height;
	vertex[41] = z - xSize / 2;

	vertex[42] = x - xSize / 2;
	vertex[43] = 0;
	vertex[44] = z + xSize / 2;

	vertex[45] = x - xSize / 2;
	vertex[46] = height;
	vertex[47] = z - xSize / 2;

	vertex[48] = x - xSize / 2;
	vertex[49] = height;
	vertex[50] = z + xSize / 2;

	vertex[51] = x - xSize / 2;
	vertex[52] = 0;
	vertex[53] = z + xSize / 2;
	// ¿Þ¸é end

	//¾Õ¸é start
	vertex[54] = x - xSize / 2;
	vertex[55] = 0;
	vertex[56] = z + xSize / 2;

	vertex[57] = x - xSize / 2;
	vertex[58] = height;
	vertex[59] = z + xSize / 2;

	vertex[60] = x + xSize / 2;
	vertex[61] = 0;
	vertex[62] = z + xSize / 2;

	vertex[63] = x - xSize / 2;
	vertex[64] = height;
	vertex[65] = z + xSize / 2;

	vertex[66] = x + xSize / 2;
	vertex[67] = height;
	vertex[68] = z + xSize / 2;

	vertex[69] = x + xSize / 2;
	vertex[70] = 0;
	vertex[71] = z + xSize / 2;
	// ¾Õ¸é end

	//À­¸é start
	vertex[72] = x - xSize / 2;
	vertex[73] = height;
	vertex[74] = z - xSize / 2;

	vertex[75] = x - xSize / 2;
	vertex[76] = height;
	vertex[77] = z + xSize / 2;

	vertex[78] = x + xSize / 2;
	vertex[79] = height;
	vertex[80] = z - xSize / 2;

	vertex[81] = x + xSize / 2;
	vertex[82] = height;
	vertex[83] = z - xSize / 2;

	vertex[84] = x - xSize / 2;
	vertex[85] = height;
	vertex[86] = z + xSize / 2;

	vertex[87] = x + xSize / 2;
	vertex[88] = height;
	vertex[89] = z + xSize / 2;
	// À­¸é end
}
