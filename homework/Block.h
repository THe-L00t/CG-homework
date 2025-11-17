#pragma once
#include <array>

class Block
{
public:
	Block(float, float, float, float);

	void update();
	void draw(GLuint);

private:
	float x{}, z{};
	float xSize{}, zSize{};

	float height{};
	float maxH{}, minH{};

	bool isVisible{ true };

	std::array<float, 30> vertex{};
};

