#version 330 core

out vec4 outColor;

uniform vec4 u_Color;

void main()
{
	outColor = u_Color;
}