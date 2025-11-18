#include "headHeader.h"
#include "shader.h"
#include "gameTimer.h"
#include <algorithm>

namespace beginConfig {
	int width{ 800 };
	int height{ 800 };
	Color bg{ 0.5f, 0.5f, 0.5f, 1.0f };  // 회색 배경
}
namespace CameraConfig {
	glm::vec3 pos{ 5.f,5.f,-2.f };
	glm::vec3 dir{ 0,0,0 };
	glm::vec3 up{ 0,1,0 };
}

namespace ViewfConfig {
	float fovy{ glm::radians(45.f) };
	float aspect{ 1.f };
	float n{ 1.f };
	float f{ 100.f };
}

GameTimer* GameTimer::Instance;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid RanColor(Color&);
GLvoid Mouse(int, int, int, int);
GLvoid MouseMotion(int, int);
GLvoid loop(int);
GLvoid initializeScene();
GLvoid generateMaze();
GLvoid createSphere();
GLvoid updateSphereVertex();
GLvoid SpecialKeys(int, int, int);
GLvoid SpecialKeysUp(int, int, int);


// Shader
Shader basic{};
GLuint VAO{};
GLuint VBO{};
std::vector<float> Vertex{};

GameTimer gt;

int num{};
int method{};
Color bg = beginConfig::bg;
int xcnt{}, ycnt{};

// 블럭 관련
#include "Block.h"
std::vector<Block> blocks;

// 구 객체 관련
bool sphereExists = false;
float sphereX = 0.0f, sphereY = 0.1f, sphereZ = 0.0f;
float sphereRadius = 0.08f;  // 미로보다 작게
float sphereVelocityX = 0.0f, sphereVelocityZ = 0.0f;
float sphereAcceleration = 10.0f;  // 가속도 (높임)
float sphereFriction = 0.92f;  // 마찰력
std::vector<float> sphereVertex;

// 방향키 상태
bool keyUp = false;
bool keyDown = false;
bool keyLeft = false;
bool keyRight = false;

// 투영 방식
bool isPerspective = true;

// 카메라 이동 및 블럭 움직임 제어
float cameraZ = 0.0f;
bool blocksMoving = true;
float cameraAngle = 0.0f;
bool rotatingY = false;
int rotationDirection = 0;
bool blocksAtMinHeight = false;  // 모든 블럭을 최소 높이로 고정
float blockSpeedMultiplier = 1.0f;  // 블럭 수직 이동 속도 배율

// 카메라 모드 (0: 기본, 1: 1인칭, 3: 3인칭)
int cameraMode = 0;
float mouseYaw = 0.0f;    // 마우스 좌우 회전 (Y축)
float mousePitch = 0.0f;  // 마우스 상하 회전 (X축)
int lastMouseX = 400;
int lastMouseY = 400;
bool firstMouse = true;


void main(int argc, char** argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(beginConfig::width, beginConfig::height);
	glutCreateWindow("Example1");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Unable to initialize GLEW" << std::endl
			;
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "GLEW Initialized\n";
	}
	if (!basic.CompileShader("basic.vs","basic.fs")) {
		std::cerr << "Shader compilation failed!" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Shader compiled successfully!" << std::endl;

	std::cout << "\n===== 명령어 안내 =====\n";
	std::cout << "o: 메인 뷰포트를 직각 투영으로 전환\n";
	std::cout << "p: 메인 뷰포트를 원근 투영으로 전환\n";
	std::cout << "z: 원근 투영 시 카메라 Z축 양의 방향으로 이동\n";
	std::cout << "Z: 원근 투영 시 카메라 Z축 음의 방향으로 이동\n";
	std::cout << "m: 블럭 움직임 토글 (정지/재생)\n";
	std::cout << "y: 카메라 Y축 양의 방향으로 공전\n";
	std::cout << "Y: 카메라 Y축 음의 방향으로 공전\n";
	std::cout << "r: 미로 생성 (블럭 일부 제거하여 미로 만들기)\n";
	std::cout << "s: 빨간 구 객체 생성 (미로 생성 후)\n";
	std::cout << "v: 모든 블럭을 최소 높이로 토글\n";
	std::cout << "+: 블럭 수직 이동 속도 증가\n";
	std::cout << "-: 블럭 수직 이동 속도 감소\n";
	std::cout << "1: 1인칭 시점 (마우스로 시점 조작)\n";
	std::cout << "3: 3인칭 쿼터뷰 시점\n";
	std::cout << "5: 기본 시점으로 복귀\n";
	std::cout << "c: 초기화 및 재시작\n";
	std::cout << "q: 프로그램 종료\n";
	std::cout << "=======================\n\n";

	initializeScene();

	// 평면 정점 데이터 설정 (위치 3개 + 색상 4개 = 7개씩)
	// 4x4 크기 평면 (-2 ~ 2), 초록색
	Vertex = {
		// 삼각형 1
		-2.0f, 0.0f, -2.0f,  0.0f, 0.8f, 0.0f, 1.0f,  // 왼쪽 뒤
		 2.0f, 0.0f, -2.0f,  0.0f, 0.8f, 0.0f, 1.0f,  // 오른쪽 뒤
		 2.0f, 0.0f,  2.0f,  0.0f, 0.8f, 0.0f, 1.0f,  // 오른쪽 앞
		// 삼각형 2
		-2.0f, 0.0f, -2.0f,  0.0f, 0.8f, 0.0f, 1.0f,  // 왼쪽 뒤
		 2.0f, 0.0f,  2.0f,  0.0f, 0.8f, 0.0f, 1.0f,  // 오른쪽 앞
		-2.0f, 0.0f,  2.0f,  0.0f, 0.8f, 0.0f, 1.0f   // 왼쪽 앞
	};

	// VAO, VBO 설정
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertex.size() * sizeof(float), Vertex.data(), GL_STATIC_DRAW);

	// 위치 속성 (location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// 색상 속성 (location = 1)
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	std::cout << "VAO/VBO setup complete!" << std::endl;

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeys);
	glutSpecialUpFunc(SpecialKeysUp);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(MouseMotion);
	glutTimerFunc(1, loop, 1);
	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(bg.r, bg.g, bg.b, bg.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// 셰이더 사용
	basic.Use();

	int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
	int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

	// 변환 행렬 설정
	glm::mat4 world = glm::mat4(1.0f);

	// ========== 메인 뷰 (전체 화면) ==========
	glViewport(0, 0, windowWidth, windowHeight);

	// 카메라 모드에 따른 view 행렬 설정
	glm::mat4 view;
	glm::vec3 cameraPos;

	if (cameraMode == 1 && sphereExists)  // 1인칭
	{
		// 구의 위치에 카메라 배치 (약간 위로)
		cameraPos = glm::vec3(sphereX, sphereY + 0.15f, sphereZ);

		// 마우스 회전에 따른 시선 방향 계산
		glm::vec3 direction;
		direction.x = cos(glm::radians(mouseYaw)) * cos(glm::radians(mousePitch));
		direction.y = sin(glm::radians(mousePitch));
		direction.z = sin(glm::radians(mouseYaw)) * cos(glm::radians(mousePitch));
		direction = glm::normalize(direction);

		glm::vec3 target = cameraPos + direction;
		view = glm::lookAt(cameraPos, target, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (cameraMode == 3 && sphereExists)  // 3인칭 쿼터뷰
	{
		// 구 뒤쪽 위에서 내려다보는 시점
		cameraPos = glm::vec3(sphereX - 1.5f, sphereY + 2.0f, sphereZ - 1.5f);
		glm::vec3 target = glm::vec3(sphereX, sphereY, sphereZ);
		view = glm::lookAt(cameraPos, target, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else  // 기본 시점 (cameraMode == 0)
	{
		cameraPos = CameraConfig::pos;
		cameraPos.z += cameraZ;

		// Y축 공전 적용
		float radius = glm::length(glm::vec2(cameraPos.x, cameraPos.z));
		cameraPos.x = radius * cos(cameraAngle);
		cameraPos.z = radius * sin(cameraAngle) + cameraZ;

		view = glm::lookAt(cameraPos, CameraConfig::dir, CameraConfig::up);
	}

	glm::mat4 projection;

	if (isPerspective)
	{
		// 1인칭 모드일 때 near plane을 매우 작게 설정
		float nearPlane = (cameraMode == 1) ? 0.01f : ViewfConfig::n;
		projection = glm::perspective(ViewfConfig::fovy, ViewfConfig::aspect, nearPlane, ViewfConfig::f);
	}
	else
	{
		projection = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, 0.1f, 100.0f);
	}

	basic.setUniform("worldT", world);
	basic.setUniform("viewT", view);
	basic.setUniform("projectionT", projection);

	// 평면 그리기
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertex.size() * sizeof(float), Vertex.data(), GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// 블럭들 그리기 (제거되지 않은 블럭만)
	glBindVertexArray(VAO);
	for (auto& block : blocks) {
		if (!block.isRemoved) {
			block.draw(VBO);
			glDrawArrays(GL_TRIANGLES, 0, 30);
		}
	}
	glBindVertexArray(0);

	// 구 그리기
	if (sphereExists && !sphereVertex.empty())
	{
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sphereVertex.size() * sizeof(float), sphereVertex.data(), GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, sphereVertex.size() / 7);
		glBindVertexArray(0);
	}

	// ========== 미니맵 뷰 (오른쪽 상단, 직각 투영) ==========
	int miniMapSize = static_cast<int>(windowWidth * 0.3f);
	int miniMapX = windowWidth - miniMapSize;
	int miniMapY = windowHeight - miniMapSize;

	glViewport(miniMapX, miniMapY, miniMapSize, miniMapSize);

	// 위에서 내려다보는 직각 투영 설정
	glm::mat4 orthoView = glm::lookAt(
		glm::vec3(0.0f, 10.0f, 0.0f),  // 카메라 위치 (위에서)
		glm::vec3(0.0f, 0.0f, 0.0f),   // 보는 방향 (원점)
		glm::vec3(0.0f, 0.0f, -1.0f)   // 업 벡터 (Z축 방향)
	);
	glm::mat4 orthoProjection = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, 0.1f, 20.0f);

	basic.setUniform("worldT", world);
	basic.setUniform("viewT", orthoView);
	basic.setUniform("projectionT", orthoProjection);

	// 미니맵 영역만 검은색으로 클리어 (Scissor Test 사용)
	glEnable(GL_SCISSOR_TEST);
	glScissor(miniMapX, miniMapY, miniMapSize, miniMapSize);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	// 평면 다시 그리기
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertex.size() * sizeof(float), Vertex.data(), GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// 블럭들 다시 그리기 (제거되지 않은 블럭만)
	glBindVertexArray(VAO);
	for (auto& block : blocks) {
		if (!block.isRemoved) {
			block.draw(VBO);
			glDrawArrays(GL_TRIANGLES, 0, 30);
		}
	}
	glBindVertexArray(0);

	// 구 그리기 (미니맵)
	if (sphereExists && !sphereVertex.empty())
	{
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sphereVertex.size() * sizeof(float), sphereVertex.data(), GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, sphereVertex.size() / 7);
		glBindVertexArray(0);
	}

	// 뷰포트 원래대로 복구
	glViewport(0, 0, windowWidth, windowHeight);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}


GLvoid Keyboard(unsigned char key, int x, int y)
{

	switch (key) {
	case 'o':
		isPerspective = false;
		std::cout << "메인 뷰포트: 직각 투영으로 전환\n";
		break;
	case 'p':
		isPerspective = true;
		std::cout << "메인 뷰포트: 원근 투영으로 전환\n";
		break;
	case 'z':
		if (isPerspective)
		{
			cameraZ += 0.5f;
			std::cout << "카메라 Z축 이동: " << cameraZ << "\n";
		}
		break;
	case 'Z':
		if (isPerspective)
		{
			cameraZ -= 0.5f;
			std::cout << "카메라 Z축 이동: " << cameraZ << "\n";
		}
		break;
	case 'm':
		blocksMoving = !blocksMoving;
		std::cout << "블럭 움직임: " << (blocksMoving ? "재생" : "정지") << "\n";
		break;
	case 'y':
		if (rotatingY && rotationDirection == 1)
		{
			rotatingY = false;
			std::cout << "카메라 Y축 공전 정지\n";
		}
		else
		{
			rotatingY = true;
			rotationDirection = 1;
			std::cout << "카메라 Y축 양의 방향 공전 시작\n";
		}
		break;
	case 'Y':
		if (rotatingY && rotationDirection == -1)
		{
			rotatingY = false;
			std::cout << "카메라 Y축 공전 정지\n";
		}
		else
		{
			rotatingY = true;
			rotationDirection = -1;
			std::cout << "카메라 Y축 음의 방향 공전 시작\n";
		}
		break;
	case '1':
		cameraMode = 1;
		mouseYaw = 0.0f;
		mousePitch = 0.0f;
		firstMouse = true;
		std::cout << "1인칭 시점 전환 (마우스로 시점 조작)\n";
		break;
	case '2':

		break;
	case '3':
		cameraMode = 3;
		std::cout << "3인칭 쿼터뷰 시점 전환\n";
		break;
	case '4':

		break;
	case '5':
		cameraMode = 0;
		std::cout << "기본 시점으로 복귀\n";
		break;
	case '6':

		break;
	case '7':

		break;
	case '8':

		break;
	case '9':

		break;
	case '0':

		break;
	case 'r':
		generateMaze();
		break;
	case 's':
		createSphere();
		break;
	case 'v':
		blocksAtMinHeight = !blocksAtMinHeight;
		if (blocksAtMinHeight)
		{
			std::cout << "모든 블럭을 최소 높이로 내리는 중...\n";
		}
		else
		{
			std::cout << "블럭 높이 변화 재개\n";
		}
		break;
	case '+':
	case '=':  // Shift 없이 +를 누르는 경우
		blockSpeedMultiplier += 0.2f;
		if (blockSpeedMultiplier > 5.0f) blockSpeedMultiplier = 5.0f;
		std::cout << "블럭 속도 배율: " << blockSpeedMultiplier << "x\n";
		break;
	case '-':
	case '_':
		blockSpeedMultiplier -= 0.2f;
		if (blockSpeedMultiplier < 0.1f) blockSpeedMultiplier = 0.1f;
		std::cout << "블럭 속도 배율: " << blockSpeedMultiplier << "x\n";
		break;
	case 'c':
		std::cout << "\n=== 초기화 시작 ===\n";
		initializeScene();
		std::cout << "=== 초기화 완료 ===\n";
		break;
	case 't':

		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

//GLvoid RanColor(Color& c)
//{
//	float r = urgb(dre);
//	float g = urgb(dre);
//	float b = urgb(dre);
//	c = { r / 255,g / 255,b / 255,1 };
//}

GLvoid Mouse(int button, int state, int x, int y)
{
	int once{ 0 };
	float mpx{ (static_cast<float>(x) - 400) / 400 }, mpy{ (static_cast<float>(-y) + 400) / 400 };
	if (state == GLUT_DOWN) {
		if (once == 0) {


			once = 1;
		}
	}
	if (state == GLUT_UP) {
		std::cout << "upȣ��" << std::endl;
		once = 0;
	}
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (once == 1) {
			once = 2;
		}
		break;
	case GLUT_MIDDLE_BUTTON:
		break;
	case GLUT_RIGHT_BUTTON:

		break;
	}
}

GLvoid MouseMotion(int x, int y)
{
	// 1인칭 모드일 때만 마우스로 시점 조작
	if (cameraMode != 1) return;

	if (firstMouse)
	{
		lastMouseX = x;
		lastMouseY = y;
		firstMouse = false;
		return;
	}

	float xoffset = x - lastMouseX;
	float yoffset = lastMouseY - y;  // y 좌표는 반대

	lastMouseX = x;
	lastMouseY = y;

	float sensitivity = 0.15f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	mouseYaw += xoffset;
	mousePitch += yoffset;

	// pitch 제한 (위아래 90도 이내)
	if (mousePitch > 89.0f) mousePitch = 89.0f;
	if (mousePitch < -89.0f) mousePitch = -89.0f;

	glutPostRedisplay();
}

GLvoid loop(int v)
{
	gt.Update();

	// Y축 공전 처리
	if (rotatingY)
	{
		cameraAngle += rotationDirection * gt.deltaTime * 1.0f;
	}

	// 블럭 움직임이 활성화된 경우에만 업데이트
	if (blocksMoving && !blocksAtMinHeight)
	{
		for (auto& block : blocks)
		{
			block.update(gt.deltaTime, blockSpeedMultiplier);
		}
	}

	// 최소 높이로 내리기 모드
	if (blocksAtMinHeight)
	{
		for (auto& block : blocks)
		{
			if (block.height > block.minH)
			{
				block.height -= gt.deltaTime * 2.0f;  // 빠르게 내려가도록
				if (block.height < block.minH)
				{
					block.height = block.minH;
				}
			}
			else
			{
				block.height = block.minH;
			}
			// vertex 갱신
			block.updateVertex();
		}
	}

	// 구 물리 업데이트
	if (sphereExists)
	{
		// 키 상태에 따라 가속도 적용
		if (cameraMode == 1)
		{
			// 1인칭 모드: 카메라가 바라보는 방향 기준으로 이동
			glm::vec3 forward;
			forward.x = cos(glm::radians(mouseYaw));
			forward.z = sin(glm::radians(mouseYaw));
			forward = glm::normalize(forward);

			glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
			glm::vec3 realForward = glm::normalize(glm::cross(glm::vec3(0.f, 1.f, 0.f), right));

			if (keyUp)  // 앞
			{
				sphereVelocityX += realForward.x * sphereAcceleration * gt.deltaTime;
				sphereVelocityZ += realForward.z * sphereAcceleration * gt.deltaTime;
			}
			if (keyDown)  // 뒤
			{
				sphereVelocityX -= realForward.x * sphereAcceleration * gt.deltaTime;
				sphereVelocityZ -= realForward.z * sphereAcceleration * gt.deltaTime;
			}
			if (keyLeft)  // 왼쪽
			{
				sphereVelocityX -= right.x * sphereAcceleration * gt.deltaTime;
				sphereVelocityZ -= right.z * sphereAcceleration * gt.deltaTime;
			}
			if (keyRight)  // 오른쪽
			{
				sphereVelocityX += right.x * sphereAcceleration * gt.deltaTime;
				sphereVelocityZ += right.z * sphereAcceleration * gt.deltaTime;
			}
		}
		else
		{
			// 기본/3인칭 모드: 월드 좌표 기준 이동
			if (keyUp)
			{
				sphereVelocityZ += sphereAcceleration * gt.deltaTime;
			}
			if (keyDown)
			{
				sphereVelocityZ -= sphereAcceleration * gt.deltaTime;
			}
			if (keyLeft)
			{
				sphereVelocityX += sphereAcceleration * gt.deltaTime;
			}
			if (keyRight)
			{
				sphereVelocityX -= sphereAcceleration * gt.deltaTime;
			}
		}

		// 마찰력 적용
		sphereVelocityX *= sphereFriction;
		sphereVelocityZ *= sphereFriction;

		// 속도가 너무 작으면 0으로
		if (fabs(sphereVelocityX) < 0.001f) sphereVelocityX = 0.0f;
		if (fabs(sphereVelocityZ) < 0.001f) sphereVelocityZ = 0.0f;

		// 최대 속도 제한
		float maxSpeed = 3.0f;
		if (fabs(sphereVelocityX) > maxSpeed) sphereVelocityX = sphereVelocityX > 0 ? maxSpeed : -maxSpeed;
		if (fabs(sphereVelocityZ) > maxSpeed) sphereVelocityZ = sphereVelocityZ > 0 ? maxSpeed : -maxSpeed;

		// 새 위치 계산
		float newX = sphereX + sphereVelocityX * gt.deltaTime;
		float newZ = sphereZ + sphereVelocityZ * gt.deltaTime;

		// 바닥 평면 경계 체크 (-2.0 ~ 2.0)
		float planeMinX = -2.0f + sphereRadius;
		float planeMaxX = 2.0f - sphereRadius;
		float planeMinZ = -2.0f + sphereRadius;
		float planeMaxZ = 2.0f - sphereRadius;

		// 경계를 벗어나면 위치 제한 및 속도 반전
		if (newX < planeMinX)
		{
			newX = planeMinX;
			sphereVelocityX *= -0.5f;
		}
		else if (newX > planeMaxX)
		{
			newX = planeMaxX;
			sphereVelocityX *= -0.5f;
		}

		if (newZ < planeMinZ)
		{
			newZ = planeMinZ;
			sphereVelocityZ *= -0.5f;
		}
		else if (newZ > planeMaxZ)
		{
			newZ = planeMaxZ;
			sphereVelocityZ *= -0.5f;
		}

		// 충돌 감지
		float blockWidth = 4.0f / xcnt;
		float blockDepth = 4.0f / ycnt;
		int gridX = static_cast<int>((newX + 2.0f) / blockWidth);
		int gridY = static_cast<int>((newZ + 2.0f) / blockDepth);

		bool canMove = false;
		if (gridX >= 0 && gridX < xcnt && gridY >= 0 && gridY < ycnt)
		{
			int index = gridX * ycnt + gridY;
			if (index >= 0 && index < blocks.size() && blocks[index].isRemoved)
			{
				canMove = true;
			}
		}

		if (canMove)
		{
			sphereX = newX;
			sphereZ = newZ;
			updateSphereVertex();
		}
		else
		{
			// 벽에 부딪히면 속도 반전 (튕김)
			sphereVelocityX *= -0.5f;
			sphereVelocityZ *= -0.5f;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(1, loop, 1);
}

GLvoid initializeScene()
{
	int newXcnt, newYcnt;

	std::cout << "\n몇 x 몇 개의 기둥들을 생성하시겠습니까? (5~25): ";
	std::cout.flush();

	if (std::cin >> newXcnt >> newYcnt)
	{
		xcnt = std::max(5, std::min(25, newXcnt));
		ycnt = std::max(5, std::min(25, newYcnt));
	}
	else
	{
		std::cin.clear();
		std::cin.ignore(10000, '\n');
		std::cout << "잘못된 입력입니다. 기본값 10x10 사용\n";
		xcnt = 10;
		ycnt = 10;
	}

	std::cout << "생성할 블럭: " << xcnt << " x " << ycnt << std::endl;

	blocks.clear();

	float blockWidth = 4.0f / xcnt;
	float blockDepth = 4.0f / ycnt;
	float startX = -2.0f + blockWidth / 2.0f;
	float startZ = -2.0f + blockDepth / 2.0f;

	for (int i = 0; i < xcnt; ++i)
	{
		for (int j = 0; j < ycnt; ++j)
		{
			float x = startX + i * blockWidth;
			float z = startZ + j * blockDepth;
			blocks.emplace_back(x, z, blockWidth, blockDepth);
		}
	}

	isPerspective = true;
	cameraZ = 0.0f;
	blocksMoving = true;
	cameraAngle = 0.0f;
	rotatingY = false;
	rotationDirection = 0;
	blocksAtMinHeight = false;
	sphereExists = false;
	keyUp = keyDown = keyLeft = keyRight = false;
	blockSpeedMultiplier = 1.0f;
	cameraMode = 0;
	mouseYaw = 0.0f;
	mousePitch = 0.0f;
	firstMouse = true;

	std::cout << "총 블럭 생성: " << blocks.size() << "개\n";
}

GLvoid generateMaze()
{
	if (blocks.empty() || xcnt == 0 || ycnt == 0)
	{
		std::cout << "블럭이 없습니다. 먼저 초기화하세요.\n";
		return;
	}

	std::cout << "\n=== 미로 생성 시작 ===\n";

	// 모든 블럭을 벽으로 초기화 (제거하지 않음)
	for (auto& block : blocks)
	{
		block.isRemoved = false;
	}

	// 방문 여부를 추적하는 2D 배열
	std::vector<std::vector<bool>> visited(xcnt, std::vector<bool>(ycnt, false));

	// 스택 기반 DFS로 미로 경로 생성
	std::vector<std::pair<int, int>> stack;

	static std::random_device rd;
	static std::mt19937 gen(rd());

	// 시작점: 좌상단 (0, 0) - 입구
	int startX = 0, startY = 0;
	stack.push_back({startX, startY});
	visited[startX][startY] = true;
	blocks[startX * ycnt + startY].isRemoved = true;  // 통로로 만듦 (입구)

	// 방향: 상, 하, 좌, 우
	std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

	while (!stack.empty())
	{
		auto [cx, cy] = stack.back();

		// 이웃 중 방문하지 않은 셀 찾기 (2칸 떨어진 셀)
		std::vector<std::pair<int, int>> neighbors;
		for (auto [dx, dy] : directions)
		{
			int nx = cx + dx * 2;
			int ny = cy + dy * 2;

			if (nx >= 0 && nx < xcnt && ny >= 0 && ny < ycnt && !visited[nx][ny])
			{
				neighbors.push_back({nx, ny});
			}
		}

		if (!neighbors.empty())
		{
			// 랜덤하게 이웃 선택
			std::uniform_int_distribution<int> dist(0, neighbors.size() - 1);
			int idx = dist(gen);
			auto [nx, ny] = neighbors[idx];

			// 중간 벽 제거 (현재 셀과 선택한 셀 사이)
			int mx = cx + (nx - cx) / 2;
			int my = cy + (ny - cy) / 2;
			blocks[mx * ycnt + my].isRemoved = true;

			// 선택한 이웃을 방문 처리하고 통로로 만듦
			visited[nx][ny] = true;
			blocks[nx * ycnt + ny].isRemoved = true;

			// 스택에 추가
			stack.push_back({nx, ny});
		}
		else
		{
			// 더 이상 방문할 이웃이 없으면 백트래킹
			stack.pop_back();
		}
	}

	// 출구: 우하단 (xcnt-1, ycnt-1) - 명시적으로 통로로 설정
	int endX = xcnt - 1, endY = ycnt - 1;
	blocks[endX * ycnt + endY].isRemoved = true;

	// 제거된 블럭 수 카운트
	int removedCount = 0;
	for (const auto& block : blocks)
	{
		if (block.isRemoved) removedCount++;
	}

	std::cout << "미로 생성 완료!\n";
	std::cout << "입구: (0, 0), 출구: (" << endX << ", " << endY << ")\n";
	std::cout << "제거된 블럭(통로): " << removedCount << "개 / 전체: " << blocks.size() << "개\n";
	std::cout << "남은 벽(블럭): " << (blocks.size() - removedCount) << "개\n";
	std::cout << "=== 미로 생성 종료 ===\n\n";
}

GLvoid createSphere()
{
	if (blocks.empty() || xcnt == 0 || ycnt == 0)
	{
		std::cout << "먼저 블럭과 미로를 생성하세요.\n";
		return;
	}

	// 미로 입구 위치 (0, 0)에 구 배치
	float blockWidth = 4.0f / xcnt;
	float blockDepth = 4.0f / ycnt;
	float startX = -2.0f + blockWidth / 2.0f;
	float startZ = -2.0f + blockDepth / 2.0f;

	sphereX = startX;
	sphereZ = startZ;
	sphereY = 0.1f;

	updateSphereVertex();

	sphereVelocityX = 0.0f;
	sphereVelocityZ = 0.0f;
	sphereExists = true;
	std::cout << "빨간 구 객체가 미로 입구(" << sphereX << ", " << sphereZ << ")에 생성되었습니다!\n";
}

GLvoid updateSphereVertex()
{
	sphereVertex.clear();

	const int stacks = 20;
	const int slices = 20;

	for (int i = 0; i < stacks; ++i)
	{
		float phi1 = glm::pi<float>() * i / stacks;
		float phi2 = glm::pi<float>() * (i + 1) / stacks;

		for (int j = 0; j < slices; ++j)
		{
			float theta1 = 2.0f * glm::pi<float>() * j / slices;
			float theta2 = 2.0f * glm::pi<float>() * (j + 1) / slices;

			// 4개의 정점으로 2개의 삼각형 만들기
			// v1
			float x1 = sphereRadius * sin(phi1) * cos(theta1);
			float y1 = sphereRadius * cos(phi1);
			float z1 = sphereRadius * sin(phi1) * sin(theta1);

			// v2
			float x2 = sphereRadius * sin(phi1) * cos(theta2);
			float y2 = sphereRadius * cos(phi1);
			float z2 = sphereRadius * sin(phi1) * sin(theta2);

			// v3
			float x3 = sphereRadius * sin(phi2) * cos(theta2);
			float y3 = sphereRadius * cos(phi2);
			float z3 = sphereRadius * sin(phi2) * sin(theta2);

			// v4
			float x4 = sphereRadius * sin(phi2) * cos(theta1);
			float y4 = sphereRadius * cos(phi2);
			float z4 = sphereRadius * sin(phi2) * sin(theta1);

			// 삼각형 1 (v1, v2, v3)
			sphereVertex.push_back(sphereX + x1);
			sphereVertex.push_back(sphereY + y1);
			sphereVertex.push_back(sphereZ + z1);
			sphereVertex.push_back(1.0f);
			sphereVertex.push_back(0.0f);
			sphereVertex.push_back(0.0f);
			sphereVertex.push_back(1.0f);

			sphereVertex.push_back(sphereX + x2);
			sphereVertex.push_back(sphereY + y2);
			sphereVertex.push_back(sphereZ + z2);
			sphereVertex.push_back(1.0f);
			sphereVertex.push_back(0.0f);
			sphereVertex.push_back(0.0f);
			sphereVertex.push_back(1.0f);

			sphereVertex.push_back(sphereX + x3);
			sphereVertex.push_back(sphereY + y3);
			sphereVertex.push_back(sphereZ + z3);
			sphereVertex.push_back(1.0f);
			sphereVertex.push_back(0.0f);
			sphereVertex.push_back(0.0f);
			sphereVertex.push_back(1.0f);

			// 삼각형 2 (v1, v3, v4)
			sphereVertex.push_back(sphereX + x1);
			sphereVertex.push_back(sphereY + y1);
			sphereVertex.push_back(sphereZ + z1);
			sphereVertex.push_back(1.0f);
			sphereVertex.push_back(0.0f);
			sphereVertex.push_back(0.0f);
			sphereVertex.push_back(1.0f);

			sphereVertex.push_back(sphereX + x3);
			sphereVertex.push_back(sphereY + y3);
			sphereVertex.push_back(sphereZ + z3);
			sphereVertex.push_back(1.0f);
			sphereVertex.push_back(0.0f);
			sphereVertex.push_back(0.0f);
			sphereVertex.push_back(1.0f);

			sphereVertex.push_back(sphereX + x4);
			sphereVertex.push_back(sphereY + y4);
			sphereVertex.push_back(sphereZ + z4);
			sphereVertex.push_back(1.0f);
			sphereVertex.push_back(0.0f);
			sphereVertex.push_back(0.0f);
			sphereVertex.push_back(1.0f);
		}
	}
}

GLvoid SpecialKeys(int key, int x, int y)
{
	if (!sphereExists) return;

	// 키 눌림 상태 저장
	switch (key)
	{
	case GLUT_KEY_UP:
		keyUp = true;
		break;
	case GLUT_KEY_DOWN:
		keyDown = true;
		break;
	case GLUT_KEY_LEFT:
		keyLeft = true;
		break;
	case GLUT_KEY_RIGHT:
		keyRight = true;
		break;
	}

	glutPostRedisplay();
}

GLvoid SpecialKeysUp(int key, int x, int y)
{
	if (!sphereExists) return;

	// 키 떼어짐 상태 저장
	switch (key)
	{
	case GLUT_KEY_UP:
		keyUp = false;
		break;
	case GLUT_KEY_DOWN:
		keyDown = false;
		break;
	case GLUT_KEY_LEFT:
		keyLeft = false;
		break;
	case GLUT_KEY_RIGHT:
		keyRight = false;
		break;
	}

	glutPostRedisplay();
}