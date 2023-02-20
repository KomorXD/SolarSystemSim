#version 330 core

in vec4 outColor;
in vec3 worldPos;
in vec3 vertexNormal;

out vec4 fragColor;

uniform int u_Lightning = 1;

void main()
{
	if(u_Lightning == 0)
	{
		fragColor = outColor;

		return;
	}

	vec3 lightPos = vec3(0.0, 3.0, 0.0);
	vec3 lightColor = vec3(1.0, 1.0, 1.0);

	vec3 lightDir = normalize(lightPos - worldPos);

	float ambientStrength = 0.3;
	float diffuseStrength = max(dot(normalize(vertexNormal), lightDir), 0.0);

	vec3 ambient = ambientStrength * lightColor;
	vec3 diffuse = diffuseStrength * lightColor;

	vec3 result = (ambient + diffuse) * vec3(outColor);

	fragColor = vec4(result, outColor.a);
}