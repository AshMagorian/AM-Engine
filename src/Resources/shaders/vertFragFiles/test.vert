#version 430 core

uniform mat4 in_View;
uniform mat4 in_Projection;
uniform mat4 in_Model;

layout (location = 0) in vec3 in_Position;
layout (location = 2) in vec2 in_TexCoord;
layout (location = 3) in vec3 in_Normal;

varying vec2 ex_TexCoord;
varying vec3 ex_FragPos;
varying vec3 ex_Normal;

void main()
{
	gl_Position = in_Projection * in_View * in_Model * vec4(in_Position, 1.0);
	ex_TexCoord = in_TexCoord;

	ex_Normal = mat3(in_Model) * in_Normal;
	ex_FragPos = vec3(in_Model * vec4(in_Position, 1.0));
}