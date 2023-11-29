#version 430 core

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