#version 430 core
out vec4 FragColor;

in vec4 clipSpace;
in vec2 texCoords;
in vec3 toCameraVector;

uniform sampler2D in_ReflectionTexture;
uniform sampler2D in_RefractionTexture;
uniform sampler2D in_dudvMap;
uniform sampler2D in_normalMap;
uniform sampler2D in_DepthMap;

uniform float in_MoveFactor;

uniform vec3 in_DirLightDirection;
uniform vec3 in_DirLightColor;

const float waveStrength = 0.02;
const float shineDamper = 20.0;
const float reflectivity = 0.5;

float CalcWaterDepth(vec2 refractTexCoords);

void main()
{
	vec2 ndc = (clipSpace.xy / clipSpace.w)/2.0 + 0.5;
	vec2 refractTexCoords = ndc;
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
	
	// Calculate the depth of the water
	float waterDepth = CalcWaterDepth(refractTexCoords);

	vec2 distortedTexCoords = texture(in_dudvMap, vec2(texCoords.x + in_MoveFactor, texCoords.y)).rg*0.1;
	distortedTexCoords = texCoords + vec2(distortedTexCoords.x, distortedTexCoords.y+in_MoveFactor);
	vec2 totalDistortion = (texture(in_dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength * clamp(waterDepth / 2.0 , 0.0, 1.0);
	
	refractTexCoords += totalDistortion;
	refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);
	
	reflectTexCoords += totalDistortion;
	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);
	
	vec4 reflectColor = texture(in_ReflectionTexture, reflectTexCoords);
	vec4 refractColor = texture(in_RefractionTexture, refractTexCoords);
	
	// Calculate the surface normal of the water
	vec4 normalMapColor = texture(in_normalMap, distortedTexCoords);
	vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b * 3.0, normalMapColor.g * 2.0 - 1.0);
	normal = normalize(normal);
	
	// Create the Fresnel effect for the reflections
	vec3 viewVector = normalize(toCameraVector);
	float refractiveFactor = dot(viewVector, normal);
	refractiveFactor = pow(refractiveFactor, 2.0);
	
	// Add specular highlights
	vec3 reflectedLight = reflect(normalize(in_DirLightDirection), normal);
	float specular = max(dot(reflectedLight, viewVector), 0.0);
	specular = pow(specular, shineDamper);
	vec3 specularHighlights = in_DirLightColor * specular * reflectivity * clamp(waterDepth , 0.0, 1.0);
	
	FragColor = mix(reflectColor, refractColor, refractiveFactor);
	FragColor = mix(FragColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2) + vec4(specularHighlights, 0.0);
	FragColor.a = clamp(waterDepth , 0.0, 1.0);
}

float CalcWaterDepth(vec2 refractTexCoords)
{
	float near = 0.1;
	float far = 100.0f;
	float depth = texture(in_DepthMap, refractTexCoords).r;
	float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
	depth = gl_FragCoord.z;
	float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
	float waterDepth = floorDistance - waterDistance;
	return waterDepth;
}

