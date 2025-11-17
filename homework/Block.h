#pragma once
#include <vector>
#include <random>
#include <algorithm>
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
	void setRandomProperties();

private:
	void createVertex();
	void updateVertexColors();

	float x{}, z{};
	float xSize{}, zSize{};

	float height{};
	float maxH{}, minH{};
	float hDir{ 1.0f }, speed{};

	bool isVisible{ true };

	// 낙하 관련
	float fallY{ 10.0f };  // 현재 낙하 높이
	bool isFalling{ false };  // 낙하 중인지 (처음엔 false)
	bool fallStarted{ false };  // 낙하 시작 여부
	float fallSpeed{ 0.0f };  // 낙하 속도
	float fallDuration{ 2.0f };  // 낙하 시간 (2초로 늘림)
	float fallTimer{ 0.0f };  // 경과 시간
	float startFallY{ 10.0f };  // 시작 높이

	// 색상 관련
	float baseR{}, baseG{}, baseB{};  // 기본 색상
	float currentR{}, currentG{}, currentB{};  // 현재 색상

	std::vector<float> vertex{};
};

