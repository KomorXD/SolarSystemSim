#version 430 core

struct Material
{
	vec4 color;
	float shininess;
	vec2 uvStart;
	vec2 uvEnd;
	vec2 normalUvStart;
	vec2 normalUvEnd;
	vec2 specularUvStart;
	vec2 specularUvEnd;
};

struct PointLight
{
	vec3 position;
	vec3 color;
	float intensity;
};

in VS_OUT
{
	vec3 worldPos;
	vec3 vertexNormal;
	vec3 staticNormal;
	mat3 TBN;
	Material material;
} fs_in;

out vec4 fragColor;

uniform vec3 u_ViewPos;
uniform sampler2D u_TextureAtlas;
uniform PointLight u_Lights[16];
uniform int u_ActiveLights = 0;

const vec3 DIRECTIONAL_COLOR = vec3(48.0 / 255.0, 16.0 / 255.0, 76.0 / 255.0);
const vec3 DIRECTIONAL_DIR = normalize(vec3(-0.3, -0.5, -0.6));

vec2 calcTextureUV(vec2 uvStart, vec2 uvEnd)
{
	float pi = 3.14159265358979323846;
	vec2 uv;
	uv.x = 0.5 + atan(fs_in.staticNormal.z, fs_in.staticNormal.x) / (2.0 * pi);
	uv.y = 0.5 - asin(fs_in.staticNormal.y) / pi;
	uv = uvStart + (uvEnd - uvStart) * uv;

	return uv;
}

vec3 directionalLightImpact()
{
	float directionalStrength = max(dot(fs_in.vertexNormal, -DIRECTIONAL_DIR), 0.0);

	return directionalStrength * DIRECTIONAL_COLOR;
}

vec3 lightDiffuseImpact(PointLight light, vec3 normal)
{
	float fragToLightLen = length(light.position - fs_in.worldPos);
	vec3 lightDir = normalize(light.position - fs_in.worldPos);
	float diffuseStrength = max(dot(normal, lightDir), 0.0) * step(0.0, dot(fs_in.vertexNormal, lightDir));

	return diffuseStrength * light.color * light.intensity;
}

vec3 lightSpecularImpact(PointLight light, vec3 normal)
{
	vec3 viewDir = normalize(u_ViewPos - fs_in.worldPos);
	vec3 lightDir = normalize(light.position - fs_in.worldPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), 32) * step(0.0, dot(fs_in.vertexNormal, lightDir));

	return fs_in.material.shininess * specularStrength * light.color;
}

void main()
{
	float ambientStrength = 0.1;
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	vec3 normal = texture(u_TextureAtlas, calcTextureUV(
		fs_in.material.normalUvStart, fs_in.material.normalUvEnd
	)).rgb;
	normal = normal * 2.0 - 1.0;
	normal = fs_in.TBN * normal;
	float specularFactor = texture(u_TextureAtlas, calcTextureUV(
		fs_in.material.specularUvStart, fs_in.material.specularUvEnd
	)).r;
	
	for(int i = 0; i < u_ActiveLights; i++)
	{
		diffuse += lightDiffuseImpact(u_Lights[i], normal);
		specular += lightSpecularImpact(u_Lights[i], normal) * specularFactor;
	}

	vec3 result = (ambientStrength + diffuse + specular) * fs_in.material.color.rgb;
	
	float gamma = 2.2;
	vec2 uv = calcTextureUV(fs_in.material.uvStart, fs_in.material.uvEnd);
	fragColor = vec4(result, fs_in.material.color.a);
	fragColor = texture(u_TextureAtlas, uv) * fragColor;
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / gamma));
}