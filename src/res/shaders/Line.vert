#version 430 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec4 a_Color;

uniform mat4 u_ViewProjection;

out vec4 color;
out vec3 fragPos;

void main()
{
	color = a_Color;
	fragPos = a_Pos;

	gl_Position = u_ViewProjection * vec4(a_Pos, 1.0);
}