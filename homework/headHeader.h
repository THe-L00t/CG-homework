#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <optional>
#include <sstream>
#include <filesystem>

#include <gl/glew.h> 
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

struct Color {
	float r;
	float g;
	float b;
	float a;
};