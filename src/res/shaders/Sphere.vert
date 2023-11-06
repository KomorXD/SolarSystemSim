#version 330 core

struct VertexMaterial
{
	vec4 color;
	float shininess;
	vec2 uvStart;
	vec2 uvEnd;
};

struct FragmentMaterial
{
	vec4 color;
	float shininess;
	vec2 uv;
};

layout(location = 0) in vec3 a_Position;
layout(location = 1) in mat4 a_Transform;;
layout(location = 5) in VertexMaterial a_Material;

uniform mat4 u_ViewProjection;

out vec3 worldPos;
out vec3 vertexNormal;
out FragmentMaterial outMaterial;

void main()
{
	float pi = 3.14159265358979323846;

	worldPos	 = (a_Transform * vec4(a_Position, 1.0)).xyz;
	vertexNormal = normalize(worldPos - (a_Transform * vec4(vec3(0.0), 1.0)).xyz);

	outMaterial.color = a_Material.color;
	outMaterial.shininess = a_Material.shininess;
	
	vec2 standardUV;
	standardUV.x = atan(vertexNormal.x, vertexNormal.z) / (2.0 * pi) + 0.5;
	standardUV.y = vertexNormal.y * 0.5 + 0.5;

	outMaterial.uv = a_Material.uvStart + (a_Material.uvEnd - a_Material.uvStart) * standardUV;

	gl_Position = u_ViewProjection * a_Transform * vec4(a_Position, 1.0);
}