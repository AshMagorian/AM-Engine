#version 330 core
layout (location = 0) in vec3 in_Pos;

out vec3 localPos;

uniform mat4 in_Projection;
uniform mat4 in_View;

void main()
{
	localPos = in_Pos;
	vec4 pos =  in_Projection * in_View * vec4(in_Pos, 1.0);
	gl_Position = pos.xyww;
}