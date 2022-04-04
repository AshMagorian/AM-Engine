#version 430 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gMRA;

struct MaterialTextures
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
	MaterialTextures matTextures;
	vec3 	albedo;
    vec3	normal;
    float 	metallic;
    float	roughness;
    float 	ao;

	//Parallax stuff
	float	parallax_Adjustment;
	float	parallax_heightScaleAdjustment;
	
	//   32       | 16     | 8      | 4        | 2     | 1
	//   parallax | albedo | normal | metallic | rough | ao
	// 	 if matValue is -1 then don't read the value
	int		matValue;
};

uniform PixelValue in_ColorValue[4]; //     0 = bg, 1 = red, 2 = green, 3 = blue

uniform sampler2D blendMap;
uniform int in_BlocksSurrounding;

in VS_OUT {
	vec3 Normal;
    vec2 TexCoord;
	vec2 TexCoordUnmodified; // The TexCoord unmodified by TexCoordScale
	vec3 FragPos;
	mat3 TBN;
} fs_in;

uniform float in_TexCoordScale;
uniform vec3 in_ViewPos;

//Parallax stuff
uniform int 	in_Parallax_ClipBorders;
uniform float	in_Parallax_Height;
uniform int 	in_Parallax_MinLayers;
uniform int 	in_Parallax_MaxLayers;

vec4 ReadBlendMap(vec2 unmodifiedTexCoords);
vec2 GetDepthMapValue(vec2 texCoord, vec4 blendMapValues);
vec3 GetAlbedoValue(vec2 texCoord, vec4 blendMapValues);
vec3 CalcNormal(vec2 texCoord, vec4 blendMapValues);
vec3 CalcMRA (vec2 texCoord, vec4 blendMapValues);
vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir);
float GetDepthAtPoint(vec2 currentTexCoords, vec2 currentTexCoords_blendMap);

void main()
{
	vec2 texCoord = fs_in.TexCoord;

	vec4 blendMapValues = ReadBlendMap(fs_in.TexCoordUnmodified);
	
	texCoord = GetDepthMapValue(texCoord, blendMapValues);
	
	vec3 albedoVal = GetAlbedoValue(texCoord, blendMapValues);
	vec3 normalVal = CalcNormal(texCoord, blendMapValues);
	vec3 mraVal = CalcMRA(texCoord, blendMapValues);
	
	//******************************************************************************
	
	gPosition = fs_in.FragPos;
	gNormal = normalVal;
	gAlbedo = albedoVal;
	gMRA = mraVal;
}

// Return a vec4 of all the values of the blendmap
vec4 ReadBlendMap(vec2 unmodifiedTexCoords)
{
	// Read from the blend map
	vec4 blendMapColor = texture(blendMap, unmodifiedTexCoords); 
	// to be used in the for loops
	return vec4(1.0 - (blendMapColor.r + blendMapColor.g + blendMapColor.b), vec3(blendMapColor)); 
	
}

//	Higher value = deeper
vec2 GetDepthMapValue(vec2 texCoord, vec4 blendMapValues)
{
	mat3 transposeTBN = transpose(fs_in.TBN );
	vec3 tangentViewDir = normalize(transposeTBN * (in_ViewPos - fs_in.FragPos));
	
	vec2 result = ParallaxMapping(texCoord,tangentViewDir);
	
	// Check if the fragment is out of bounds

	// top
	if (result.y > in_TexCoordScale && (in_BlocksSurrounding & 8) == 0)
		discard;
		
	// bottom
	if (result.y < 0.0 && (in_BlocksSurrounding & 4) == 0)
		discard;

	// left
	if (result.x < 0.0 && (in_BlocksSurrounding & 2 )== 0)
		discard;
		
	// right
	if(result.x > in_TexCoordScale && (in_BlocksSurrounding & 1) == 0)
		discard;

	return result;
}

vec3 GetAlbedoValue(vec2 texCoord, vec4 blendMapValues)
{
	vec3 result = vec3(0.0);
	
	// Loop through all 4 materials
	for (int i = 0; i < 4; i++)
	{
		if (in_ColorValue[i].matValue != -1 && blendMapValues[i] > 0.01)
		{
			if(in_ColorValue[i].matValue & 16)
			{
				result += pow(texture(in_ColorValue[i].matTextures.texture_diffuse1, texCoord).rgb, vec3(2.2)) * blendMapValues[i];
			}
			else
			{
				result += in_ColorValue[i].albedo * blendMapValues[i];
			}

		}
	}
	return vec3(result);
}

vec3 CalcNormal(vec2 texCoord, vec4 blendMapValues)
{
	vec3 normal = vec3(0.0);
	
	// Loop through all 4 materials
	for (int i = 0; i < 4; i++)
	{
		// Check if there is a material loaded and if it appears on the blendMap
		if (in_ColorValue[i].matValue != -1 && blendMapValues[i] > 0.01)
		{
			if(in_ColorValue[i].matValue & 8)
			{
				vec3 tmpNormal = texture(in_ColorValue[i].matTextures.texture_normal1, texCoord).rgb;
				tmpNormal = tmpNormal * 2.0 - 1.0; // makes values between -1 and 1
				tmpNormal = normalize(fs_in.TBN * tmpNormal);
				tmpNormal *=  blendMapValues[i];
				
				normal += tmpNormal;
			}
			else
			{
				normal += normalize(fs_in.Normal) * blendMapValues[i] ;
			}		
		}
	}
	return normal;
}

vec3 CalcMRA (vec2 texCoord, vec4 blendMapValues)
{
	vec3 result = vec3(0.0);
	
	// Loop through all 4 materials
	for (int i = 0; i < 4; i++)
	{
		// Check if there is a material loaded and if it appears on the blendMap
		if (in_ColorValue[i].matValue != -1 && blendMapValues[i] > 0.01)
		{
			// Check for metallic texture
			if(in_ColorValue[i].matValue & 4)
			{
				result.x += texture(in_ColorValue[i].matTextures.texture_metallic1, texCoord).r * blendMapValues[i];
			}
			else
			{
				result.x += in_ColorValue[i].metallic * blendMapValues[i];
			}
			
			// Check for roughness texture
			if(in_ColorValue[i].matValue & 2)
			{
				result.y += texture(in_ColorValue[i].matTextures.texture_roughness1, texCoord).r * blendMapValues[i];
			}
			else
			{
				result.y += in_ColorValue[i].roughness * blendMapValues[i];
			}
			
			// Check for ao texture
			if(in_ColorValue[i].matValue & 1)
			{
				result.z += texture(in_ColorValue[i].matTextures.texture_ao1, texCoord).r * blendMapValues[i];
			}
			else
			{
				result.z += in_ColorValue[i].ao * blendMapValues[i];
			}
		}
	}	
	return vec3(result);
}


vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir)
{
	float heightScale = in_Parallax_Height;															// test value
	//float height =  texture(in_Material.texture_displacement1, texCoord).r;    
    //vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
    //return texCoord - p; 
	
	// number of depth layers
    const float minLayers = in_Parallax_MinLayers;															// test value
    const float maxLayers = in_Parallax_MaxLayers;															// test value
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
	vec2 deltaTexCoords_blendMap = deltaTexCoords / in_TexCoordScale;
  
    // get initial values
    vec2  currentTexCoords     = texCoord;
	vec2 currentTexCoords_blendMap = fs_in.TexCoordUnmodified;
	
    float currentDepthMapValue = GetDepthAtPoint(currentTexCoords, currentTexCoords_blendMap);
	
	// 	return the current texCoords if there is no depthmap used
	if (currentDepthMapValue < 0.00001 && currentDepthMapValue > -0.00001)
		return currentTexCoords;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
		currentTexCoords_blendMap -= deltaTexCoords_blendMap;
		
        // get depthmap value at current texture coordinates
        currentDepthMapValue = GetDepthAtPoint(currentTexCoords, currentTexCoords_blendMap);
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
	vec2 prevTexCoords_blendMap = currentTexCoords_blendMap + deltaTexCoords_blendMap;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = GetDepthAtPoint(prevTexCoords, prevTexCoords_blendMap) - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);
	
    return vec2(finalTexCoords);
}

float GetDepthAtPoint(vec2 currentTexCoords, vec2 currentTexCoords_blendMap)
{
	vec4 blendMapValues = ReadBlendMap(currentTexCoords_blendMap);
	float result = 0.0;
	
	// Loop through all 4 materials
	for (int i = 0; i < 4; i++)
	{
		if (in_ColorValue[i].matValue != -1 && blendMapValues[i] > 0.01)
		{
			if(in_ColorValue[i].matValue & 32)
			{
				float tmp = texture(in_ColorValue[i].matTextures.texture_displacement1, currentTexCoords).r * in_ColorValue[i].parallax_heightScaleAdjustment;
				tmp += in_ColorValue[i].parallax_Adjustment;
				result += tmp * blendMapValues[i];
			}
		}
	}
	return result;
}
