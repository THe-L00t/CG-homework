#version 330

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec4 a_Color;

out vec4 v_Color;

uniform mat4 projectionT;
uniform mat4 viewT;
uniform mat4 worldT;

void main()
{
	vec4 newPosition = projectionT * viewT * worldT * vec4(a_Pos,1);

	gl_Position = newPosition;
	v_Color = a_Color;
}