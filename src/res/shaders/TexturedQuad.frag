#version 330 core

in vec2 textureCoords;

out vec4 fragColor;

uniform sampler2D u_Texture;

void main()
{
	fragColor = texture(u_Texture, textureCoords);
}