#version 330 core

struct Material
{
	vec4 color;
	float shininess;
};

layout(location = 0) in vec3 a_Position;
layout(location = 1) in mat4 a_Transform;;
layout(location = 5) in Material a_Material;

uniform mat4 u_ViewProjection;

out vec3 worldPos;
out vec3 vertexNormal;
out Material outMaterial;

void main()
{
	worldPos	 = (a_Transform * vec4(a_Position, 1.0)).xyz;
	vertexNormal = normalize(worldPos - (a_Transform * vec4(vec3(0.0), 1.0)).xyz);
	outMaterial  = a_Material;

	gl_Position = u_ViewProjection * a_Transform * vec4(a_Position, 1.0);
}