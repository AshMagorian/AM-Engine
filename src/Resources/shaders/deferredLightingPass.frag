#version 330 core
out vec4 FragColor;

struct PixelValue
{
	vec3 albedo;
    vec3 normal;
    float metallic;
    float roughness;
    float ao;
};

struct DirLight
{
	vec3 direction;
	vec3 color;
};

struct PointLight
{
	vec3 position;
	vec3 color;
};

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMRA;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT;  

uniform vec3 in_ViewPos;
uniform DirLight in_DirLight;
uniform int in_NoPointLights;
uniform PointLight in_PointLights[50];

const float PI = 3.14159265359;

PixelValue GetGBufferData();
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, PixelValue pixel, vec3 F0);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, PixelValue pixel, vec3 F0);

vec3 ReflectanceCalculation(vec3 L, vec3 H, vec3 radiance, vec3 normal, vec3 viewDir, PixelValue pixel, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 F0);
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);


void main()
{             
	PixelValue pixel = GetGBufferData();
	vec3 fragPos = texture(gPosition, TexCoords).rgb;
	vec3 viewDir = normalize(in_ViewPos - fragPos); // V
	vec3 F0 = vec3(0.04); // Surface reflection at zero incidence
	F0 = mix(F0, pixel.albedo, pixel.metallic);
	
	vec3 Lo = vec3(0.0);
	Lo += CalcDirLight(in_DirLight, pixel.normal, viewDir, pixel, F0);
	for(int i = 0; i < in_NoPointLights; i++)
	{
		Lo += CalcPointLight(in_PointLights[i], pixel.normal, fragPos, viewDir, pixel, F0); 
	}
	
	//******** DIFFUSE IBL *************
	vec3 F = FresnelSchlickRoughness(max(dot(pixel.normal, viewDir), 0.0), F0, pixel.roughness);
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= (1.0 - pixel.metallic);
	vec3 irradiance = texture(irradianceMap, pixel.normal).rgb;
	vec3 diffuse = irradiance * pixel.albedo;	
	
	//******** SPECULAR IBL *************
	vec3 R = reflect(-viewDir, pixel.normal);
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefilteredColor = textureLod(prefilterMap, R,  pixel.roughness * MAX_REFLECTION_LOD).rgb; 
	vec2 envBRDF  = texture(brdfLUT, vec2(max(dot(pixel.normal, viewDir), 0.0), pixel.roughness)).rg;
	vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
	
	vec3 ambient =  (kD * diffuse + specular) * pixel.ao; 
	vec3 color = ambient + Lo;
	
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2));
	FragColor = vec4(color, 1.0);
}

PixelValue GetGBufferData()
{
	// retrieve data from gbuffer
	PixelValue rtn;
    rtn.normal = texture(gNormal, TexCoords).rgb;
    rtn.albedo = texture(gAlbedo, TexCoords).rgb;
    rtn.metallic = texture(gMRA, TexCoords).r;
	rtn.roughness =  texture(gMRA, TexCoords).g;
	rtn.ao = texture(gMRA, TexCoords).b;
	return rtn;
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, PixelValue pixel, vec3 F0)
{
	vec3 L = normalize(-in_DirLight.direction);      	// Light direction
	vec3 H = normalize(viewDir + L);            	 	// Inbetween vector
	vec3 radiance = light.color;	
	
	return ReflectanceCalculation(L, H, radiance, normal, viewDir, pixel, F0);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, PixelValue pixel, vec3 F0)
{
	// calculate radiance 
	vec3 L = normalize(light.position - fragPos); 		// Light direction
	vec3 H = normalize(viewDir + L);              		// Inbetween vector
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (distance * distance);
	vec3 radiance = light.color * attenuation;
	
	return ReflectanceCalculation(L, H, radiance, normal, viewDir, pixel, F0);
}

vec3 ReflectanceCalculation(vec3 L, vec3 H, vec3 radiance, vec3 normal, vec3 viewDir, PixelValue pixel, vec3 F0)
{
	// cook-torrance BRDF
	float NDF = DistributionGGX(normal, H, pixel.roughness);
	float G = GeometrySmith(normal, viewDir, L, pixel.roughness);
	vec3 F = FresnelSchlick(max(dot(H, viewDir), 0.0), F0);
	
	vec3 kS = F;						//Specular ratio
	vec3 kD = vec3(1.0) - kS;			//Diffuse ratio
	kD *= 1.0 - pixel.metallic;
	
	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, L), 0.0);
	vec3 specular = numerator / max(denominator, 0.0001);
	
	//add outgoing radiance Lo usijng the reflectance equation
	float NdotL = max(dot(normal, L), 0.0);
	vec3 rtn = (kD * pixel.albedo / PI + specular) * radiance * NdotL;
	return rtn;
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}