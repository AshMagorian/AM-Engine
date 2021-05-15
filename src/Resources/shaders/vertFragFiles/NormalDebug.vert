#version 430 core

layout (location = 0) in vec3 in_Position;
layout (location = 2) in vec2 in_TexCoord;
layout (location = 3) in vec3 in_Normal;
layout (location = 4) in vec3 in_Tangent;

out VS_OUT {
    vec3 Normal;
} vs_out;

uniform mat4 in_View;
uniform mat4 in_Projection;
uniform mat4 in_Model;

void main()
{
	gl_Position = in_Projection * in_View * in_Model * vec4(in_Position, 1.0);
	vs_out.Normal = mat3(in_Model) * in_Normal;

}