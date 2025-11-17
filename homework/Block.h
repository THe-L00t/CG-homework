#pragma once
#include <vector>
#include <gl/glew.h> 
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

class Block
{
public:
	Block(float, float, float, float);

	void update(float);
	void draw(GLuint);

private:
	void createVertex();

	float x{}, z{};
	float xSize{}, zSize{};

	float height{};
	float maxH{}, minH{};
	float hDir{}, speed{};

	bool isVisible{ true };

	std::vector<float> vertex{};
};

