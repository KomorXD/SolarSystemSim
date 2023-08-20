#version 330 core

in vec3 TexCoords;

uniform samplerCube u_SkyboxTex;

out vec4 outColor;

void main()
{
	vec4 voidColor = vec4(vec3(0.0), 1.0);
	float threshold = 0.1;

	if(TexCoords.y > 0.0)
	{
		outColor = texture(u_SkyboxTex, TexCoords);
	}
	else if(TexCoords.y < -threshold)
	{
		outColor = voidColor;
	}
	else
	{
		outColor = mix(voidColor, texture(u_SkyboxTex, TexCoords), (TexCoords.y + threshold) * 10);
	}
}