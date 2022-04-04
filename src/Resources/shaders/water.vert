#version 430 core

uniform mat4 in_View;
uniform mat4 in_Projection;
uniform mat4 in_Model;

uniform vec3 in_CameraPos;

out vec4 clipSpace;
out vec2 texCoords;
out vec3 toCameraVector;

layout (location = 0) in vec2 in_Position;

const float tiling = 8.0;

void main()
{
	vec4 worldPosition = in_Model * vec4(in_Position.x, 0.0, in_Position.y, 1.0);
	clipSpace = in_Projection * in_View * worldPosition;
	gl_Position = clipSpace;
	texCoords = vec2(in_Position.x/2.0 + 0.5, in_Position.y/2.0 + 0.5) * tiling;
	toCameraVector = in_CameraPos - worldPosition.xyz;
}