#version 430 core

out vec4 FragColor;

in VS_OUT {
    vec3 Normal;
} fs_in;

void main()
{
	FragColor = vec4(fs_in.Normal, 1.0);
}
