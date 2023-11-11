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
layout(location = 5) in Material a_Material;

uniform mat4 u_ViewProjection;

out VS_OUT
{
	vec3 worldPos;
	vec3 vertexNormal;
	vec3 staticNormal;
	Material material;
} vs_out;

void main()
{
	vs_out.worldPos = (a_Transform * vec4(a_Position, 1.0)).xyz;
	vs_out.vertexNormal = normalize(vs_out.worldPos - (a_Transform * vec4(vec3(0.0), 1.0)).xyz);
	vs_out.staticNormal = normalize(a_Position - vec3(0.0));
	vs_out.material = a_Material;

	gl_Position = u_ViewProjection * a_Transform * vec4(a_Position, 1.0);
}