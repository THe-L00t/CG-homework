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
GLvoid loop(int);


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

// 투영 방식
bool isPerspective = true;


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
	std::cout << "q: 프로그램 종료\n";
	std::cout << "=======================\n\n";

	std::cout << "몇 x 몇 개의 기둥들을 생성하시겠습니까? (5~25): ";
	std::cin >> xcnt >> ycnt;

	// 입력 범위 제한
	xcnt = std::max(5, std::min(25, xcnt));
	ycnt = std::max(5, std::min(25, ycnt));
	std::cout << "생성할 블럭: " << xcnt << " x " << ycnt << std::endl;

	// 블럭 생성 (평면을 빈 공간 없이 꽉 채움)
	float blockWidth = 4.0f / xcnt;   // X 방향 블럭 크기 (평면 가로 길이 / xcnt)
	float blockDepth = 4.0f / ycnt;   // Z 방향 블럭 크기 (평면 세로 길이 / ycnt)
	float startX = -2.0f + blockWidth / 2.0f;
	float startZ = -2.0f + blockDepth / 2.0f;

	for (int i = 0; i < xcnt; ++i) {
		for (int j = 0; j < ycnt; ++j) {
			float x = startX + i * blockWidth;
			float z = startZ + j * blockDepth;
			blocks.emplace_back(x, z, blockWidth, blockDepth);  // 간격 없이 꽉 채움
		}
	}

	std::cout << "Total blocks created: " << blocks.size() << std::endl;

	// 평면 정점 데이터 설정 (위치 3개 + 색상 4개 = 7개씩)
	// 4x4 크기 평면 (-2 ~ 2), 흰색
	Vertex = {
		// 삼각형 1
		-2.0f, 0.0f, -2.0f,  1.0f, 1.0f, 1.0f, 1.0f,  // 왼쪽 뒤
		 2.0f, 0.0f, -2.0f,  1.0f, 1.0f, 1.0f, 1.0f,  // 오른쪽 뒤
		 2.0f, 0.0f,  2.0f,  1.0f, 1.0f, 1.0f, 1.0f,  // 오른쪽 앞
		// 삼각형 2
		-2.0f, 0.0f, -2.0f,  1.0f, 1.0f, 1.0f, 1.0f,  // 왼쪽 뒤
		 2.0f, 0.0f,  2.0f,  1.0f, 1.0f, 1.0f, 1.0f,  // 오른쪽 앞
		-2.0f, 0.0f,  2.0f,  1.0f, 1.0f, 1.0f, 1.0f   // 왼쪽 앞
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
	glutMouseFunc(Mouse);
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

	// ========== 메인 뷰 (전체 화면) ==========
	glViewport(0, 0, windowWidth, windowHeight);

	// 변환 행렬 설정
	glm::mat4 world = glm::mat4(1.0f);
	glm::mat4 view = glm::lookAt(CameraConfig::pos, CameraConfig::dir, CameraConfig::up);
	glm::mat4 projection;

	if (isPerspective)
	{
		projection = glm::perspective(ViewfConfig::fovy, ViewfConfig::aspect, ViewfConfig::n, ViewfConfig::f);
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
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// 블럭들 그리기
	glBindVertexArray(VAO);
	for (auto& block : blocks) {
		block.draw(VBO);
		glDrawArrays(GL_TRIANGLES, 0, 30);
	}
	glBindVertexArray(0);

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

	// 미니맵에 테두리 효과 (검은색 배경)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	// 평면 다시 그리기
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// 블럭들 다시 그리기
	glBindVertexArray(VAO);
	for (auto& block : blocks) {
		block.draw(VBO);
		glDrawArrays(GL_TRIANGLES, 0, 30);
	}
	glBindVertexArray(0);

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
	case '1':

		break;
	case '2':

		break;
	case '3':

		break;
	case '4':

		break;
	case '5':

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
	case 'c':

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

GLvoid loop(int v)
{
	gt.Update();

	// 모든 블럭 업데이트
	for (auto& block : blocks) {
		block.update(gt.deltaTime);
	}

	glutPostRedisplay();
	glutTimerFunc(1, loop, 1);
}