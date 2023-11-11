#version 330 core

struct Material
{
	vec4 color;
	float shininess;
	vec2 uvStart;
	vec2 uvEnd;
};

struct PointLight
{
	vec4 color;
	float intensity;
};

in VS_OUT
{
	vec3 worldPos;
	vec3 vertexNormal;
	vec3 staticNormal;
	Material material;
} fs_in;

out vec4 fragColor;

uniform sampler2D u_TextureAtlas;
uniform PointLight u_Lights[16];
uniform int u_ActiveLights = 0;

const int SHADING_LEVELS = 3;

void main()
{
	float gamma = 2.2;
	float pi = 3.14159265358979323846;
	vec2 uv;
	uv.x = 0.5 + atan(fs_in.staticNormal.z, fs_in.staticNormal.x) / (2.0 * pi);
	uv.y = 0.5 - asin(fs_in.staticNormal.y) / pi;
	uv = fs_in.material.uvStart + (fs_in.material.uvEnd - fs_in.material.uvStart) * uv;

	vec4 matColor = fs_in.material.color;
	float matShininess = fs_in.material.shininess;

	vec3 dirLightVec = normalize(vec3(-0.3, -0.5, -0.6));
	vec3 lightPos = vec3(6.0, 10.0, 7.0);
	vec3 lightColor = vec3(1.0, 1.0, 1.0);

	float fragToLightLen = length(lightPos - fs_in.worldPos);
	vec3 lightDir = normalize(lightPos - fs_in.worldPos);

	float directionalStrength = max(dot(fs_in.vertexNormal, -dirLightVec), 0.0);
	float ambientStrength = 0.1;
	float diffuseStrength = max(dot(normalize(fs_in.vertexNormal), lightDir), 0.0);
	float attenuation = 1.0 / (fragToLightLen * fragToLightLen);
	
	float diffuseLevel = floor(diffuseStrength * SHADING_LEVELS);
	float directionalLevel = floor(directionalStrength * SHADING_LEVELS);
	
	diffuseStrength = diffuseLevel / SHADING_LEVELS;
	directionalStrength = directionalLevel / SHADING_LEVELS;

	vec3 ambient = (ambientStrength * lightColor);
	vec3 diffuse = diffuseStrength * lightColor;
	vec3 result = (ambient + diffuse + attenuation) * vec3(matColor) + directionalStrength * vec3(48.0 / 255.0, 16.0 / 255.0, 76.0 / 255.0);

	fragColor = vec4(result, matColor.a);
	fragColor = texture(u_TextureAtlas, uv) * fragColor;
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / gamma));
}