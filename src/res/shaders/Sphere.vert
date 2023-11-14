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

out VS_OUT
{
	out vec3 worldPos;
	out vec3 vertexNormal;
	out vec3 staticNormal;
	out Material material;
} vs_out;

uniform mat4 u_ViewProjection;

void main()
{
	vs_out.worldPos = (a_Transform * vec4(a_Position, 1.0)).xyz;
	vs_out.vertexNormal = normalize(vs_out.worldPos - (a_Transform * vec4(vec3(0.0), 1.0)).xyz);
	vs_out.staticNormal = normalize(a_Position - vec3(0.0));
	vs_out.material.color = a_MaterialColor;
	vs_out.material.shininess = a_MaterialShininess;
	vs_out.material.uvStart = a_MaterialUvStart;
	vs_out.material.uvEnd = a_MaterialUvEnd;

	gl_Position = u_ViewProjection * a_Transform * vec4(a_Position, 1.0);
}