#version 330 core

struct Material
{
	vec4 color;
	float shininess;
	vec2 uvStart;
	vec2 uvEnd;
};

in vec3 worldPos;
in vec3 vertexNormal;
in vec3 staticNormal;
in Material material;

out vec4 fragColor;

uniform sampler2D u_TextureAtlas;

const int SHADING_LEVELS = 3;

void main()
{
	float gamma = 2.2;
	float pi = 3.14159265358979323846;
	vec2 uv;
	uv.x = 0.5 + atan(staticNormal.z, staticNormal.x) / (2.0 * pi);
	uv.y = 0.5 - asin(staticNormal.y) / pi;
	uv = material.uvStart + (material.uvEnd - material.uvStart) * uv;

	fragColor = texture(u_TextureAtlas, uv) * material.color;
	// fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / gamma));
}