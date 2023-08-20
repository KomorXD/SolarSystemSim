#version 330 core

uniform vec3 u_CameraPos;

in vec4 color;
in vec3 fragPos;

out vec4 fragColor;

void main()
{
	float fadeFactor = 0.0;
	float dist = distance(fragPos, vec3(u_CameraPos.x, 0.0, u_CameraPos.z));

	if(dist > 150.0)
	{
		fadeFactor = 1.0 - (200.0 - dist) / 50.0;
	}

	fragColor = vec4(color.xyz, 1.0 - fadeFactor);
}