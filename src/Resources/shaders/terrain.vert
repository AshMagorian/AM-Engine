#version 430 core

layout (location = 0) in vec3 in_Position;
layout (location = 2) in vec2 in_TexCoord;
layout (location = 3) in vec3 in_Normal;
layout (location = 4) in vec3 in_Tangent;

out VS_OUT {
    vec3 Normal;
    vec2 TexCoord;
	vec3 FragPos;
	mat3 TBN;
} vs_out;

uniform float in_TexCoordScale;
uniform mat4 in_View;
uniform mat4 in_Projection;
uniform mat4 in_Model;

void main()
{
	vs_out.FragPos = vec3(in_Model * vec4(in_Position, 1.0));
	vs_out.TexCoord = in_TexCoord * in_TexCoordScale;
	vec3 normal =  in_Normal;  
	vs_out.Normal = normal;
	gl_Position = in_Projection * in_View * in_Model * vec4(in_Position, 1.0);
	
	vec3 T = normalize(vec3(in_Model * vec4(in_Tangent, 0.0)));
    vec3 N = normalize(normal);
	vec3 B = cross(N, T);
	vs_out.TBN = mat3(T, B, N);
	
}