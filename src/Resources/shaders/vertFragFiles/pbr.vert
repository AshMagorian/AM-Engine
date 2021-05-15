#version 430 core

layout (location = 0) in vec3 in_Position;
layout (location = 2) in vec2 in_TexCoord;
layout (location = 3) in vec3 in_Normal;
layout (location = 4) in vec3 in_Tangent;

struct DirLight
{
	vec3 direction;
	vec3 color;
};
uniform DirLight in_DirLight;

uniform vec3 in_ViewPos;

out VS_OUT {
    vec3 Normal;
    vec2 TexCoord;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
	vec3 DirLightTangentSpace;
	vec3 PointLightTangentSpace[50];
	
} vs_out;

uniform mat4 in_View;
uniform mat4 in_Projection;
uniform mat4 in_Model;

void main()
{
	vs_out.TexCoord = in_TexCoord;
	vs_out.Normal = mat3(in_Model) * in_Normal;
	gl_Position = in_Projection * in_View * in_Model * vec4(in_Position, 1.0);
	
	vec3 T = normalize(vec3(in_Model * vec4(in_Tangent, 0.0)));
    vec3 N = normalize(vec3(in_Model * vec4(in_Normal, 0.0)));
	vec3 B = cross(N, T);
    mat3 TBN = transpose(mat3(T, B, N));
	//mat3 TBN = mat3(1.0);
	
	vs_out.TangentFragPos = TBN * vec3(in_Model * vec4(in_Position, 1.0));
	vs_out.TangentViewPos = TBN * in_ViewPos;
	vs_out.DirLightTangentSpace = TBN * (-in_DirLight.direction);
}