#version 430 core

out vec4 FragColor;

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_normal1;
	sampler2D texture_metallic1;
	sampler2D texture_roughness1;
	sampler2D texture_ao1;
	sampler2D texture_displacement1;
};

struct TexelValue
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

struct SpotLight
{
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;
	float constant;
	float linear;
	float quadratic;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in VS_OUT {
	vec3 Normal;
    vec2 TexCoord;
	vec3 FragPos;
	mat3 TBN;
} fs_in;

uniform float in_TexCoordScale;
uniform int in_Parallax_ClipBorders;
uniform float in_Parallax_Height;
uniform int in_Parallax_MinLayers;
uniform int in_Parallax_MaxLayers;

uniform vec3 in_ViewPos;
uniform int in_NoPointLights;
uniform int in_NoSpotLights;

uniform DirLight in_DirLight;
uniform PointLight in_PointLights[50];
uniform SpotLight in_SpotLights[50];

uniform int in_MatBinary;
uniform TexelValue in_Tex;
uniform Material in_Material;
const float PI = 3.14159265359;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT;  

TexelValue CalcTexelValues();
vec3 CalcNormal(vec2 texCoord);
vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, TexelValue tex, vec3 F0);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, TexelValue tex, vec3 F0);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, TexelValue tex);

vec3 ReflectanceCalculation(vec3 L, vec3 H, vec3 radiance, vec3 normal, vec3 viewDir, TexelValue tex, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 F0);
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

void main()
{
	TexelValue tex = CalcTexelValues();

	vec3 viewDir = normalize(in_ViewPos - fs_in.FragPos); // V
	vec3 F0 = vec3(0.04); // Surface reflection at zero incidence
	F0 = mix(F0, tex.albedo, tex.metallic);
	
	vec3 Lo = vec3(0.0);
	Lo += CalcDirLight(in_DirLight, tex.normal, viewDir, tex, F0);
	for(int i = 0; i < in_NoPointLights; i++)
	{
		Lo += CalcPointLight(in_PointLights[i], tex.normal, fs_in.FragPos, viewDir, tex, F0); 
	}
	for (int i = 0; i < in_NoSpotLights; i++)
	{
		Lo += CalcSpotLight(in_SpotLights[i], tex.normal, fs_in.FragPos, viewDir, tex);
	}
	
	//******** DIFFUSE IBL *************
	vec3 F = FresnelSchlickRoughness(max(dot(tex.normal, viewDir), 0.0), F0, tex.roughness);
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= (1.0 - tex.metallic);
	vec3 irradiance = texture(irradianceMap, tex.normal).rgb;
	vec3 diffuse = irradiance * tex.albedo;	
	
	//******** SPECULAR IBL *************
	vec3 R = reflect(-viewDir, tex.normal);
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefilteredColor = textureLod(prefilterMap, R,  tex.roughness * MAX_REFLECTION_LOD).rgb; 
	vec2 envBRDF  = texture(brdfLUT, vec2(max(dot(tex.normal, viewDir), 0.0), tex.roughness)).rg;
	vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
	
	vec3 ambient =  (kD * diffuse + specular) * tex.ao; 
	vec3 color = ambient + Lo;
	
	//vec3 ambient = vec3(0.03) * tex.albedo * tex.ao;
	//vec3 color = ambient + Lo;
	
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2));

	FragColor = vec4(color, 1.0);
}

TexelValue CalcTexelValues()
{
	int matBinary = in_MatBinary;
	TexelValue rtn = in_Tex;
	vec2 texCoord = fs_in.TexCoord;
	
	if(matBinary >= 32)
	{	mat3 transposeTBN = transpose(fs_in.TBN );
		vec3 tangentViewDir = normalize(transposeTBN * (in_ViewPos - fs_in.FragPos));
		texCoord = ParallaxMapping(fs_in.TexCoord, tangentViewDir);
		if (in_Parallax_ClipBorders == 1)
		{
			if(texCoord.x > in_TexCoordScale || texCoord.y > in_TexCoordScale || texCoord.x < 0.0 || texCoord.y < 0.0)
				discard;
		}
		matBinary -= 32;
	}
	if(matBinary >= 16)
	{	rtn.albedo    = pow(texture(in_Material.texture_diffuse1, texCoord).rgb, vec3(2.2));
		matBinary -= 16;
	}
	if(matBinary >= 8)
	{	rtn.normal    = CalcNormal(texCoord);
		matBinary -= 8;
	}
	else
		rtn.normal    = normalize(fs_in.Normal);
	if(matBinary >= 4)
	{	rtn.metallic  = texture(in_Material.texture_metallic1, texCoord).r;
		matBinary -= 4;
	}
	if(matBinary >= 2)
	{	rtn.roughness = texture(in_Material.texture_roughness1, texCoord).r;
		matBinary -= 2;
	}
	if(matBinary >= 1)
		rtn.ao        = texture(in_Material.texture_ao1, texCoord).r;
	return rtn;
}

vec3 CalcNormal(vec2 texCoord)
{
	vec3 normal = texture(in_Material.texture_normal1, texCoord).rgb;
	normal = normal * 2.0 - 1.0;
	normal = normalize(fs_in.TBN * normal);

	return normal;
}

vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir)
{
	float heightScale = in_Parallax_Height;
	//float height =  texture(in_Material.texture_displacement1, texCoord).r;    
    //vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
    //return texCoord - p; 
	
	// number of depth layers
    const float minLayers = in_Parallax_MinLayers;
    const float maxLayers = in_Parallax_MaxLayers;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoord;
    float currentDepthMapValue = texture(in_Material.texture_displacement1, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(in_Material.texture_displacement1, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(in_Material.texture_displacement1, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, TexelValue tex, vec3 F0)
{
	vec3 L = normalize(-in_DirLight.direction);      	// Light direction
	vec3 H = normalize(viewDir + L);            	 	// Inbetween vector
	vec3 radiance = light.color;	
	
	return ReflectanceCalculation(L, H, radiance, normal, viewDir, tex, F0);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, TexelValue tex, vec3 F0)
{
	// calculate radiance 
	vec3 L = normalize(light.position - fragPos); 		// Light direction
	vec3 H = normalize(viewDir + L);              		// Inbetween vector
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (distance * distance);
	vec3 radiance = light.color * attenuation;
	
	return ReflectanceCalculation(L, H, radiance, normal, viewDir, tex, F0);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, TexelValue tex)
{
	vec3 lightDir = normalize(light.position - fragPos);
	return vec3(0.0, 0.0, 0.0);
}

vec3 ReflectanceCalculation(vec3 L, vec3 H, vec3 radiance, vec3 normal, vec3 viewDir, TexelValue tex, vec3 F0)
{
	// cook-torrance BRDF
	float NDF = DistributionGGX(normal, H, tex.roughness);
	float G = GeometrySmith(normal, viewDir, L, tex.roughness);
	vec3 F = FresnelSchlick(max(dot(H, viewDir), 0.0), F0);
	
	vec3 kS = F;						//Specular ratio
	vec3 kD = vec3(1.0) - kS;			//Diffuse ratio
	kD *= 1.0 - tex.metallic;
	
	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, L), 0.0);
	vec3 specular = numerator / max(denominator, 0.0001);
	
	//add outgoing radiance Lo usijng the reflectance equation
	float NdotL = max(dot(normal, L), 0.0);
	vec3 rtn = (kD * tex.albedo / PI + specular) * radiance * NdotL;
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