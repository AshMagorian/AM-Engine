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
	//vs_out.Normal = mat3(in_Model) * in_Tangent;
	
	vec3 tmp = normalize(in_Tangent);
	(tmp.x + 1.0) / 2.0;
	(tmp.y + 1.0) / 2.0;
	(tmp.z + 1.0) / 2.0;
	
	vec3 T = normalize(vec3(in_Model * vec4(in_Tangent, 0.0)));
    vec3 N = normalize(vec3(in_Model * vec4(in_Normal, 0.0)));
	vec3 B = cross(N, T);
	
	vs_out.Normal = T;

}