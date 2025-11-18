#include "Block.h"
#include <iostream>
#include <algorithm>

Block::Block(float ix, float iz, float xS, float zS)
	: x(ix), z(iz), xSize(xS), zSize(zS)
{
	vertex.reserve(210);
	setRandomProperties();

	currentR = baseR;
	currentG = baseG;
	currentB = baseB;

	createVertex();
}

void Block::setRandomProperties()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());

	std::uniform_real_distribution<float> colorDist(0.2f, 0.9f);
	std::uniform_real_distribution<float> minHeightDist(0.05f, 0.5f);
	std::uniform_real_distribution<float> heightDist(0.5f, 2.5f);
	std::uniform_real_distribution<float> speedDist(0.3f, 1.5f);
	std::uniform_real_distribution<float> delayDist(0.0f, 8.0f);

	baseR = colorDist(gen);
	baseG = colorDist(gen);
	baseB = colorDist(gen);

	minH = minHeightDist(gen);
	maxH = heightDist(gen);
	height = minH;

	speed = speedDist(gen);

	hDir = 1.0f;

	startFallY = 10.0f;
	fallY = startFallY;
	isFalling = false;
	fallStarted = false;
	fallSpeed = 0.0f;
	fallDuration = 5.0f;
	fallTimer = 0.0f;
	fallDelay = delayDist(gen);
}

void Block::update(float deltaT)
{
	update(deltaT, 1.0f);
}

void Block::update(float deltaT, float speedMultiplier)
{
	if (!fallStarted)
	{
		fallDelay -= deltaT;
		currentR = baseR;
		currentG = baseG;
		currentB = baseB;

		if (fallDelay <= 0.0f)
		{
			isFalling = true;
			fallStarted = true;
			fallTimer = 0.0f;
		}
	}
	else if (isFalling)
	{
		fallTimer += deltaT;

		float t = fallTimer / fallDuration;
		if (t > 1.0f)
		{
			t = 1.0f;
		}

		fallY = startFallY * (1.0f - t);

		currentR = baseR;
		currentG = baseG;
		currentB = baseB;

		if (fallTimer >= fallDuration)
		{
			fallY = 0.0f;
			isFalling = false;
		}
	}
	else
	{
		height += hDir * deltaT * speed * speedMultiplier;

		if (height > maxH)
		{
			height = maxH;
			hDir = -1.0f;
		}
		else if (height < minH)
		{
			height = minH;
			hDir = 1.0f;
		}

		float t = (height - minH) / (maxH - minH);
		currentR = baseR + 0.6f * t;
		currentG = baseG + 0.6f * t;
		currentB = baseB + 0.6f * t;

		if (currentR > 1.0f) currentR = 1.0f;
		if (currentG > 1.0f) currentG = 1.0f;
		if (currentB > 1.0f) currentB = 1.0f;
	}

	vertex.clear();
	createVertex();
}

void Block::updateVertexColors()
{
	for (size_t i = 0; i < vertex.size(); i += 7)
	{
		vertex[i + 3] = currentR;
		vertex[i + 4] = currentG;
		vertex[i + 5] = currentB;
		vertex[i + 6] = 1.0f;
	}
}

void Block::updateVertex()
{
	vertex.clear();
	createVertex();
}

void Block::draw(GLuint VBO)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), vertex.data(), GL_STATIC_DRAW);
}

void Block::createVertex()
{
	float yOffset = (!fallStarted || isFalling) ? fallY : 0.0f;

	vertex.push_back(x - xSize / 2);
	vertex.push_back(yOffset);
	vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(yOffset + height);
	vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(yOffset);
	vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(yOffset + height);
	vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(yOffset + height);
	vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(yOffset);
	vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(yOffset);
	vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(yOffset + height);
	vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(yOffset);
	vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(yOffset + height);
	vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(yOffset + height);
	vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(yOffset);
	vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(yOffset);
	vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(yOffset + height);
	vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(yOffset);
	vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(yOffset + height);
	vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(yOffset + height);
	vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(yOffset);
	vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(yOffset);
	vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(yOffset + height);
	vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(yOffset);
	vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(yOffset + height);
	vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(yOffset + height);
	vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(yOffset);
	vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(yOffset + height);
	vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(yOffset + height);
	vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(yOffset + height);
	vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(yOffset + height);
	vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x - xSize / 2);
	vertex.push_back(yOffset + height);
	vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);

	vertex.push_back(x + xSize / 2);
	vertex.push_back(yOffset + height);
	vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR);
	vertex.push_back(currentG);
	vertex.push_back(currentB);
	vertex.push_back(1.0f);
}
