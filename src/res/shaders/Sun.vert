#version 330 core

struct Material
{
	vec4 color;
	float shininess;
	vec2 uvStart;
	vec2 uvEnd;
};

layout(location = 0)  in vec3  a_Position;
layout(location = 1)  in vec3  a_Tangent;
layout(location = 2)  in vec3  a_Bitangent;
layout(location = 3)  in mat4  a_Transform;
layout(location = 7)  in vec4  a_MaterialColor;
layout(location = 8)  in float a_MaterialShininess;
layout(location = 9)  in vec2  a_MaterialUvStart;
layout(location = 10) in vec2  a_MaterialUvEnd;

uniform mat4 u_ViewProjection;

out VS_OUT
{
	vec3 staticNormal;
	Material material;
} vs_out;

void main()
{
	vs_out.staticNormal = normalize(a_Position - vec3(0.0));
	vs_out.material.color = a_MaterialColor;
	vs_out.material.shininess = a_MaterialShininess;
	vs_out.material.uvStart = a_MaterialUvStart;
	vs_out.material.uvEnd = a_MaterialUvEnd;

	gl_Position = u_ViewProjection * a_Transform * vec4(a_Position, 1.0);
}