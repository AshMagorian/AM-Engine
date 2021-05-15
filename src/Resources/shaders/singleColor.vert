#version 430 core

uniform mat4 in_View;
uniform mat4 in_Projection;
uniform mat4 in_Model;

layout (location = 0) in vec3 in_Position;
layout (location = 3) in vec3 in_Normal;

uniform int assimpMesh;
uniform vec3 inverseScale;

void main()
{
	vec4 pos;
	if (assimpMesh == 1)
		pos = in_Projection * in_View * in_Model * vec4(in_Position + (inverseScale * in_Normal*0.02), 1.0);
	else
		pos =  in_Projection * in_View * in_Model * vec4(in_Position, 1.0);
	gl_Position = pos;
	
	//gl_Position = in_Projection * in_View * in_Model * vec4(in_Position, 1.0);
	//gl_Position = in_Projection * in_View * in_Model * vec4(in_Position + (inverseScale * in_Normal*0.02), 1.0);
}