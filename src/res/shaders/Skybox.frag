#version 330 core

in vec3 TexCoords;

uniform samplerCube u_SkyboxTex;

out vec4 outColor;

void main()
{
	outColor = texture(u_SkyboxTex, TexCoords);
}