#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Color;

uniform mat4 u_ViewProjection;

out vec3 worldPos;
out vec3 vertexNormal;
out vec4 outColor;

void main()
{
	worldPos	 = a_Position;
	vertexNormal = a_Normal;
	outColor	 = a_Color;

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}