#version 330 core

struct Material
{
	vec4 color;
	float shininess;
	vec2 uvStart;
	vec2 uvEnd;
	vec2 normalUvStart;
	vec2 normalUvEnd;
};

layout(location = 0)  in vec3  a_Position;
layout(location = 1)  in vec3  a_Tangent;
layout(location = 2)  in vec3  a_Bitangent;
layout(location = 3)  in mat4  a_Transform;
layout(location = 7)  in vec4  a_MaterialColor;
layout(location = 8)  in float a_MaterialShininess;
layout(location = 9)  in vec2  a_MaterialUvStart;
layout(location = 10) in vec2  a_MaterialUvEnd;
layout(location = 11) in vec2  a_MaterialNormalUvStart;
layout(location = 12) in vec2  a_MaterialNormalUvEnd;

out VS_OUT
{
	vec3 worldPos;
	vec3 vertexNormal;
	vec3 staticNormal;
	mat3 TBN;
	Material material;
} vs_out;

uniform mat4 u_ViewProjection;

void main()
{
	vs_out.worldPos = (a_Transform * vec4(a_Position, 1.0)).xyz;
	vs_out.vertexNormal = normalize(vs_out.worldPos - (a_Transform * vec4(vec3(0.0), 1.0)).xyz);
	vs_out.staticNormal = a_Position;
	vs_out.material.color = a_MaterialColor;
	vs_out.material.shininess = a_MaterialShininess;
	vs_out.material.uvStart = a_MaterialUvStart;
	vs_out.material.uvEnd = a_MaterialUvEnd;
	vs_out.material.normalUvStart = a_MaterialNormalUvStart;
	vs_out.material.normalUvEnd = a_MaterialNormalUvEnd;

	vec3 T = normalize(vec3(a_Transform * vec4(a_Tangent,	0.0)));
	vec3 B = normalize(vec3(a_Transform * vec4(a_Bitangent, 0.0)));
	vec3 N = normalize(vec3(a_Transform * vec4(a_Position,	0.0)));
	vs_out.TBN = mat3(T, B, N);

	gl_Position = u_ViewProjection * a_Transform * vec4(a_Position, 1.0);
}