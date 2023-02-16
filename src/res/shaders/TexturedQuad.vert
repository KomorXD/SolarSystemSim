#version 330 core

layout(location = 0) in vec2 a_Pos;
layout(location = 1) in vec2 a_TextureCoods;

out vec2 textureCoords;

void main()
{
	textureCoords = a_TextureCoods;

	gl_Position = vec4(a_Pos, 0.0, 1.0);
}