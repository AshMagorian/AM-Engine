#version 430 core

struct Material
{
	sampler2D diffuse;
};

varying vec2 ex_TexCoord;
varying vec3 ex_FragPos;
varying vec3 ex_Normal;


uniform Material in_Material;

void main()
{
	vec3 norm = normalize(ex_Normal);

	//Pre-calculates the texture values for efficiency
	vec3 diffuseTex = vec3(texture(in_Material.diffuse, ex_TexCoord));

	gl_FragColor = vec4(diffuseTex, 1);
}

