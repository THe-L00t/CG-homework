#include "Block.h"
#include <iostream>

Block::Block(float ix, float iz, float xS, float zS)
	:x{ix},z{iz},xSize{xS},zSize{zS}
{
	vertex.reserve(210);  // 30 vertices * 7 (pos3 + color4)
	setRandomProperties();

	// 초기 색상 설정 (낙하 전에도 보이도록)
	currentR = baseR;
	currentG = baseG;
	currentB = baseB;

	createVertex();
}

void Block::setRandomProperties()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<float> colorDist(0.2f, 0.9f);
	static std::uniform_real_distribution<float> heightDist(0.5f, 2.0f);
	static std::uniform_real_distribution<float> speedDist(0.3f, 1.5f);

	// 랜덤 기본 색상
	baseR = colorDist(gen);
	baseG = colorDist(gen);
	baseB = colorDist(gen);

	// 랜덤 높이 범위
	minH = 0.0f;
	maxH = heightDist(gen);
	height = minH;

	// 랜덤 속도
	speed = speedDist(gen);

	// 초기 방향
	hDir = 1.0f;

	// 낙하 설정 (모든 블럭이 같은 높이에서 5초 동안 떨어짐)
	startFallY = 10.0f;  // 고정된 높이
	fallY = startFallY;
	isFalling = false;  // 처음엔 낙하하지 않음
	fallStarted = false;
	fallSpeed = 0.0f;
	fallDuration = 5.0f;
	fallTimer = 0.0f;
}

void Block::update(float deltaT)
{
	// 아직 낙하를 시작하지 않았으면 시작
	if (!fallStarted) {
		isFalling = true;
		fallStarted = true;
		fallTimer = 0.0f;  // 타이머 초기화
		// 낙하 중에도 기본 색상 유지
		currentR = baseR;
		currentG = baseG;
		currentB = baseB;
	}

	// 낙하 중일 때
	if (isFalling) {
		fallTimer += deltaT;

		// 5초 동안 선형 보간으로 떨어지기
		float t = fallTimer / fallDuration;  // 0~1 사이 값
		if (t > 1.0f) t = 1.0f;

		fallY = startFallY * (1.0f - t);  // 시작 높이에서 0으로 선형 감소

		// 낙하 중에도 기본 색상 유지
		currentR = baseR;
		currentG = baseG;
		currentB = baseB;

		// 5초가 지나면 낙하 종료
		if (fallTimer >= fallDuration) {
			fallY = 0.0f;
			isFalling = false;
		}
	}
	// 낙하 전이면 색상만 설정
	else if (!fallStarted) {
		currentR = baseR;
		currentG = baseG;
		currentB = baseB;
	}
	// 바닥에 도착했을 때만 상하 운동
	else if (fallStarted) {  // 낙하를 시작했고 끝난 경우에만
		height += hDir * deltaT * speed;
		if (height > maxH) {
			height = maxH;
			hDir = -1.0f;
		}
		else if (height < minH) {
			height = minH;
			hDir = 1.0f;
		}

		// 높이에 따라 색상 변화 (높을수록 원래 색에서 0.6만큼 더 밝게)
		float t = (height - minH) / (maxH - minH);  // 0~1 사이 값
		currentR = std::min(baseR + 0.6f * t, 1.0f);
		currentG = std::min(baseG + 0.6f * t, 1.0f);
		currentB = std::min(baseB + 0.6f * t, 1.0f);
	}

	vertex.clear();
	createVertex();
}

void Block::updateVertexColors()
{
	// 정점 데이터에서 색상만 업데이트
	for (size_t i = 0; i < vertex.size(); i += 7) {
		vertex[i + 3] = currentR;
		vertex[i + 4] = currentG;
		vertex[i + 5] = currentB;
		vertex[i + 6] = 1.0f;
	}
}

void Block::draw(GLuint VBO)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), vertex.data(), GL_STATIC_DRAW);
}

void Block::createVertex()
{
	// 낙하 오프셋: 낙하 중이거나 시작 전이면 fallY만큼 위로, 착지 후엔 0
	float yOffset = (!fallStarted || isFalling) ? fallY : 0.0f;

	//뒷면 start
	// v1 (바닥)
	vertex.push_back(x - xSize / 2); vertex.push_back(yOffset); vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	// v2 (윗면)
	vertex.push_back(x - xSize / 2); vertex.push_back(yOffset + height); vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	// v3
	vertex.push_back(x + xSize / 2); vertex.push_back(yOffset); vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	// v4
	vertex.push_back(x - xSize / 2); vertex.push_back(yOffset + height); vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	// v5
	vertex.push_back(x + xSize / 2); vertex.push_back(yOffset + height); vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	// v6
	vertex.push_back(x + xSize / 2); vertex.push_back(yOffset); vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	// 뒷면 end

	//오른쪽 start
	vertex.push_back(x + xSize / 2); vertex.push_back(yOffset); vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	vertex.push_back(x + xSize / 2); vertex.push_back(yOffset + height); vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	vertex.push_back(x + xSize / 2); vertex.push_back(yOffset); vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	vertex.push_back(x + xSize / 2); vertex.push_back(yOffset + height); vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	vertex.push_back(x + xSize / 2); vertex.push_back(yOffset + height); vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	vertex.push_back(x + xSize / 2); vertex.push_back(yOffset); vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	// 오른쪽 end

	//왼쪽 start
	vertex.push_back(x - xSize / 2); vertex.push_back(yOffset); vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	vertex.push_back(x - xSize / 2); vertex.push_back(yOffset + height); vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	vertex.push_back(x - xSize / 2); vertex.push_back(yOffset); vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	vertex.push_back(x - xSize / 2); vertex.push_back(yOffset + height); vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	vertex.push_back(x - xSize / 2); vertex.push_back(yOffset + height); vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	vertex.push_back(x - xSize / 2); vertex.push_back(yOffset); vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	// 왼쪽 end

	//앞면 start
	vertex.push_back(x - xSize / 2); vertex.push_back(yOffset); vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	vertex.push_back(x - xSize / 2); vertex.push_back(yOffset + height); vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	vertex.push_back(x + xSize / 2); vertex.push_back(yOffset); vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	vertex.push_back(x - xSize / 2); vertex.push_back(yOffset + height); vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	vertex.push_back(x + xSize / 2); vertex.push_back(yOffset + height); vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	vertex.push_back(x + xSize / 2); vertex.push_back(yOffset); vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	// 앞면 end

	//위쪽 start
	vertex.push_back(x - xSize / 2); vertex.push_back(yOffset + height); vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	vertex.push_back(x - xSize / 2); vertex.push_back(yOffset + height); vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	vertex.push_back(x + xSize / 2); vertex.push_back(yOffset + height); vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	vertex.push_back(x + xSize / 2); vertex.push_back(yOffset + height); vertex.push_back(z - zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	vertex.push_back(x - xSize / 2); vertex.push_back(yOffset + height); vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	vertex.push_back(x + xSize / 2); vertex.push_back(yOffset + height); vertex.push_back(z + zSize / 2);
	vertex.push_back(currentR); vertex.push_back(currentG); vertex.push_back(currentB); vertex.push_back(1.0f);
	// 위쪽 end
}
