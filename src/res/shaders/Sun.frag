#version 430 core

struct Material
{
	vec4 color;
	float shininess;
	vec2 uvStart;
	vec2 uvEnd;
	vec2 normalUvStart;
	vec2 normalUvEnd;
};

in VS_OUT
{
	vec3 staticNormal;
	Material material;
} fs_in;

out vec4 fragColor;

uniform sampler2D u_TextureAtlas;

void main()
{
	float gamma = 2.2;
	float pi = 3.14159265358979323846;
	vec2 uv;
	uv.x = 0.5 + atan(fs_in.staticNormal.z, fs_in.staticNormal.x) / (2.0 * pi);
	uv.y = 0.5 - asin(fs_in.staticNormal.y) / pi;
	uv = fs_in.material.uvStart + (fs_in.material.uvEnd - fs_in.material.uvStart) * uv;

	fragColor = texture(u_TextureAtlas, uv) * fs_in.material.color;
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / gamma));
}