#version 430 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gMRA;

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_normal1;
	sampler2D texture_metallic1;
	sampler2D texture_roughness1;
	sampler2D texture_ao1;
	sampler2D texture_displacement1;
};

struct PixelValue
{
	vec3 albedo;
    vec3 normal;
    float metallic;
    float roughness;
    float ao;
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
uniform float in_Parallax_adjustment;

uniform vec3 in_ViewPos;

uniform int in_MatValue;
uniform PixelValue in_pxlVal;
uniform Material in_Material;

PixelValue CalcPixelValues();
vec3 CalcNormal(vec2 texCoord);
vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir);

void main()
{
	PixelValue value = CalcPixelValues();
	
	gPosition = fs_in.FragPos;
	gNormal = value.normal;
	gAlbedo = value.albedo;
	gMRA.r = value.metallic;
	gMRA.g = value.roughness;
	gMRA.b = value.ao;
	
}

PixelValue CalcPixelValues()
{
	int matValue = in_MatValue;
	PixelValue rtn = in_pxlVal;
	vec2 texCoord = fs_in.TexCoord;
	
	if(matValue >= 32)
	{
		mat3 transposeTBN = transpose(fs_in.TBN );
		vec3 tangentViewDir = normalize(transposeTBN * (in_ViewPos - fs_in.FragPos));
		texCoord = ParallaxMapping(fs_in.TexCoord, tangentViewDir);
		if (in_Parallax_ClipBorders == 1)
		{
			if(texCoord.x > in_TexCoordScale || texCoord.y > in_TexCoordScale || texCoord.x < 0.0 || texCoord.y < 0.0)
				discard;
		}
		matValue -= 32;
	}
	if(matValue >= 16)
	{	rtn.albedo    = pow(texture(in_Material.texture_diffuse1, texCoord).rgb, vec3(2.2));
		matValue -= 16;
	}
	if(matValue >= 8)
	{	rtn.normal    = CalcNormal(texCoord);
		matValue -= 8;
	}
	else
		rtn.normal    = normalize(fs_in.Normal);
	if(matValue >= 4)
	{	rtn.metallic  = texture(in_Material.texture_metallic1, texCoord).r;
		matValue -= 4;
	}
	if(matValue >= 2)
	{	rtn.roughness = texture(in_Material.texture_roughness1, texCoord).r;
		matValue -= 2;
	}
	if(matValue >= 1)
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
	currentDepthMapValue += in_Parallax_adjustment;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(in_Material.texture_displacement1, currentTexCoords).r; 
		currentDepthMapValue += in_Parallax_adjustment;		
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


