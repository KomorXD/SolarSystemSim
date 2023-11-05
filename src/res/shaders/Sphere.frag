#version 330 core

struct Material
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

in vec3 worldPos;
in vec3 vertexNormal;
in vec4 outColor;
in Material outMaterial;

out vec4 fragColor;

uniform int u_Lightning = 1;

const int SHADING_LEVELS = 3;

void main()
{
	if(u_Lightning == 0)
	{
		fragColor = outColor;

		return;
	}

	vec3 dirLightVec = normalize(vec3(-0.3, -0.5, -0.6));
	vec3 lightPos = vec3(6.0, 10.0, 7.0);
	vec3 lightColor = vec3(1.0, 1.0, 1.0);

	float fragToLightLen = length(lightPos - worldPos);
	vec3 lightDir = normalize(lightPos - worldPos);

	float directionalStrength = max(dot(vertexNormal, -dirLightVec), 0.0);
	float ambientStrength = 0.1;
	float diffuseStrength = max(dot(normalize(vertexNormal), lightDir), 0.0);
	float attenuation = 1.0 / (fragToLightLen * fragToLightLen);
	
	float diffuseLevel = floor(diffuseStrength * SHADING_LEVELS);
	float directionalLevel = floor(directionalStrength * SHADING_LEVELS);
	
	diffuseStrength = diffuseLevel / SHADING_LEVELS;
	directionalStrength = directionalLevel / SHADING_LEVELS;

	vec3 ambient = (ambientStrength * lightColor);
	vec3 diffuse = diffuseStrength * lightColor;

	vec3 result = (ambient + diffuse + attenuation) * vec3(outColor) + directionalStrength * vec3(48.0 / 255.0, 16.0 / 255.0, 76.0 / 255.0);

	fragColor = vec4(result, outColor.a);

	float gamma = 2.2;

	fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / gamma));
}