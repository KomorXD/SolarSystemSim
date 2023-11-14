#version 330 core

struct Material
{
	vec4 color;
	float shininess;
	vec2 uvStart;
	vec2 uvEnd;
};

layout(location = 0) in vec3 a_Position;
layout(location = 1) in mat4 a_Transform;
layout(location = 5) in vec4 a_MaterialColor;
layout(location = 6) in float a_MaterialShininess;
layout(location = 7) in vec2 a_MaterialUvStart;
layout(location = 8) in vec2 a_MaterialUvEnd;

uniform mat4 u_ViewProjection;

out VS_OUT
{
	vec4 color;
} vs_out;

void main()
{
	vs_out.color = a_MaterialColor;

	gl_Position = u_ViewProjection * a_Transform * vec4(a_Position, 1.0);
}