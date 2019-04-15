#pragma pack_matrix(row_major)

/****************************************************************************************
	Macros
*****************************************************************************************/

#define MERGE(a, b) a##b
#define BLUR_WEIGHTS_ARRAY_SIZE 16
#define BLUR_WEIGHTS_FLOAT_COUNT 4

#define MULTI_TEXTURE_ARRAY_COUNT 2
#define MULTI_TEXTURE_FLOAT_COUNT 4
#define MULTI_TEXTURE_ARRAY_MAX MULTI_TEXTURE_ARRAY_COUNT * MULTI_TEXTURE_FLOAT_COUNT

#define MAX_LIGHTS 8

/****************************************************************************************
	Struct
*****************************************************************************************/

struct Material
{
	float4 emissiveColor;
	float4 ambientColor;
    float4 diffuseColor;
	float4 specularColor;
	float specularPower;
	float3 padding0;
};

struct PointLight
{
	float3 pos;
	float padding0;
	float range;
	float3 padding1;
	float4 diffuseColor;
	float4 specularColor;
	float atten0;
	float atten1;
	float atten2;
	float padding2;
};

struct SpotLight
{
	float3 pos;
	float padding0;
	float3 direction;
	float range;
	float innerConeRads;
	float outerConeRads;
	float2 padding1;
	float4 diffuseColor;
	float4 specularColor;
	float atten0;
	float atten1;
	float atten2;
	float padding2;
	bool shadowEnabled;
	float shadowBias;
    float distanceAdjustment;
	float padding3;
};

struct PointLightParam
{
	PointLight light;
	float3 pos;
	float3 normal;
	float2 texCoord;
	float3 cameraPos;
	Texture2D texDiffuse;
	Texture2D texSpecular;
	SamplerState texSampler;
};

struct SpotLightParam
{
	SpotLight light;
	float3 pos;
	float3 normal;
	float2 texCoord;
	float4 posLightSpace;
	float3 cameraPos;
	Texture2D texDiffuse;
	Texture2D texSpecular;
	SamplerState samplerTex;
	Texture2D texShadowMap;
	SamplerState samplerShadowMap;
};

/****************************************************************************************
	Helper function
*****************************************************************************************/

//-------------------------------------------------------------------
//	 texToNormalMap
//-------------------------------------------------------------------
float3 texToNormalMap(float3 color)
{
	return (color - 0.5f) * 2.0f;
}
//-------------------------------------------------------------------
//	 colorFade
//-------------------------------------------------------------------
float3 colorFade(float3 color, float amount)
{
	float3 avg = (color.x + color.y + color.z) / 3.0f;
	float3 result = color + (avg - color) * amount;
	return result;
}
//-------------------------------------------------------------------
//	 mtxmul
//-------------------------------------------------------------------
matrix mtxmul(matrix m1, matrix m2, matrix m3)
{
	return mul(m1, mul(m2, m3));
}
//-------------------------------------------------------------------
//	 calculateWvp
//-------------------------------------------------------------------
float4 calculateWvp(float3 pos, matrix world, matrix view, matrix proj)
{
	return mul( float4(pos, 1.0f), mtxmul(world, view, proj) );
}
//-------------------------------------------------------------------
//	 calculateTexTransform
//-------------------------------------------------------------------
float2 calculateTexTransform(float2 texCoord, matrix matrixTransf)
{
	float3x3 matrixTransf33 = (float3x3)(matrixTransf);
	return mul(float3(texCoord, 1.0f), matrixTransf33).xy;
}
//-------------------------------------------------------------------
//	 homogenousPointToTexCoord
//-------------------------------------------------------------------
float2 homogenousPointToTexCoord(float4 p)
{
	float2 result = p.xy / p.w;
	result.x = result.x * 0.5f + 0.5f;
	result.y = -result.y * 0.5f + 0.5f;
	return result;
}
//-------------------------------------------------------------------
//	 mul3
//-------------------------------------------------------------------
float3 mul3(float3 v, matrix m)
{
	float3x3 m3 = (float3x3)m;

	return mul( v, m3 );
}
//-------------------------------------------------------------------
//	 calculateAttenuation
//-------------------------------------------------------------------
float calculateAttenuation(float dist, float current, float atten0, float atten1, float atten2)
{
	float attenuation = 1.0f;
	float start = dist * atten0;

	if ( current > start )
	{
		float proportion = (current - start) / (dist - start);
		float proportionSq = proportion*proportion;
		attenuation = 1.0f / (1.0f + proportionSq * atten2);
        attenuation -= proportionSq*proportion * atten1;
		attenuation = saturate( attenuation );
	}

	return attenuation;
}
//-------------------------------------------------------------------
//	 linstep
//-------------------------------------------------------------------
float linstep(float low, float high, float v)
{
	return clamp( (v - low) / (high - low), 0.0f, 1.0f );
}
//-------------------------------------------------------------------
//	 calculatePointLight
//-------------------------------------------------------------------
float4 calculatePointLight(PointLightParam param, Material material)
{
    if ( param.light.range < 0.1f )
        return float4(0.0f, 0.0f, 0.0f, 0.0f);

    float4 textureColor = param.texDiffuse.Sample( param.texSampler, param.texCoord );
    float3 incident = param.pos - param.light.pos;
    float incidentDist = length( incident );
    float3 incidentNorm = incident / incidentDist;
    float diffuseIntensity = saturate( dot(-incidentNorm, param.normal) );

    if ( incidentDist > param.light.range )
        return float4(0.0f, 0.0f, 0.0f, 0.0f);

    float4 diffuseColor = material.diffuseColor * param.light.diffuseColor;
    float4 diffuse = diffuseColor * diffuseIntensity;

    float3 pointToCam = param.cameraPos - param.pos;
    float3 halfVector = normalize(pointToCam + -incident);
    float4 specularColorTex = param.texSpecular.Sample( param.texSampler, param.texCoord ).rgba;
    float specularIntensity = pow(saturate(dot(halfVector, param.normal)), material.specularPower * specularColorTex.a);

    float4 specular = material.specularColor * float4(specularColorTex.rgb, 1.0f) * param.light.specularColor * specularIntensity * diffuseIntensity;

    float attenuation = calculateAttenuation( param.light.range, incidentDist, param.light.atten0, param.light.atten1, param.light.atten2 );

    float4 result = (diffuse + specular) * attenuation;
    result *= textureColor;

    return result;
}
//-------------------------------------------------------------------
//	 calculateSpotLight
//-------------------------------------------------------------------
float4 calculateSpotLight(SpotLightParam param, Material material)
{
    SpotLight spotLight = param.light;

    if ( spotLight.outerConeRads + spotLight.innerConeRads < 0.008f )
        return float4(0.0f, 0.0f, 0.0f, 0.0f);

    float3 incident = param.pos - spotLight.pos;
    float incidentDist = length( incident );
    float3 incidentNorm = incident / incidentDist;
    float cosInnerCone = cos( spotLight.innerConeRads * 0.5f );
    float cosOuterCone = cos( (spotLight.outerConeRads + spotLight.innerConeRads) * 0.5f );
    float incidentDotDirection = dot(incidentNorm, spotLight.direction);

    if ( incidentDist > spotLight.range || incidentDotDirection < cosOuterCone )
        return float4(0.0f, 0.0f, 0.0f, 0.0f);

    // Shadow related
    float shadowAmount = 1.0f;
    if ( spotLight.shadowEnabled )
    {
        float4 posLightSpace = param.posLightSpace;
        posLightSpace.xyz /= posLightSpace.w;

        float distanceRatio = incidentDist / spotLight.range;

        float2 shadowMapTexCoord;
        shadowMapTexCoord.x = posLightSpace.x * 0.5f + 0.5f;
        shadowMapTexCoord.y = posLightSpace.y * -0.5f + 0.5f;

        float lightToPointDepth = posLightSpace.z;
        float shadowMapDepth = param.texShadowMap.Sample( param.samplerShadowMap, shadowMapTexCoord ).r;
        shadowAmount = step( lightToPointDepth - spotLight.shadowBias * (1.0f + distanceRatio * spotLight.distanceAdjustment), shadowMapDepth );
    }

    float4 textureColor = param.texDiffuse.Sample( param.samplerTex, param.texCoord );

    float diffuseIntensity = saturate( dot(-incidentNorm, param.normal) );

    float4 diffuseColor = material.diffuseColor * spotLight.diffuseColor;
    float4 diffuse = diffuseColor * diffuseIntensity;

    float3 pointToCam = param.cameraPos - param.pos;
    float3 halfVector = normalize(pointToCam + -incident);
    float4 specularColorTex = param.texSpecular.Sample( param.samplerTex, param.texCoord ).rgba;
    float specularIntensity = pow(saturate(dot(halfVector, param.normal)), material.specularPower * specularColorTex.a);

    float4 specular = material.specularColor * float4(specularColorTex.rgb, 1.0f) * spotLight.specularColor * specularIntensity * diffuseIntensity;

    float attenuation = calculateAttenuation( spotLight.range, incidentDist, spotLight.atten0, spotLight.atten1, spotLight.atten2 );

    float coneAttenuation = 1.0f;
    if ( incidentDotDirection < cosInnerCone )
    {
        coneAttenuation = calculateAttenuation( abs(cosOuterCone - cosInnerCone), abs(incidentDotDirection - cosInnerCone), 0.0f, spotLight.atten1, spotLight.atten2 );
    }

    attenuation *= coneAttenuation;

    float4 result = (diffuse + specular) * attenuation;
    result *= textureColor * shadowAmount;

    return result;
}

/****************************************************************************************
	Struct shader I/Output
*****************************************************************************************/

struct Input_P4C4
{
	float4 pos: POSITION;
	float4 color: COLOR0;
};

struct Input_P3C4
{
	float3 pos: POSITION;
	float4 color: COLOR0;
};

struct Input_P3T2
{
	float3 pos: POSITION;
	float2 texCoord: TEXCOORD0;
};

struct Input_P3N3
{
	float3 pos: POSITION;
	float3 normal: NORMAL0;
};

struct Input_P3N3T2
{
	float3 pos: POSITION;
	float3 normal: NORMAL0;
	float2 texCoord: TEXCOORD0;	
};

struct Input_P3N3Tg3Bn3T2
{
	float3 pos: POSITION;
	float3 normal: NORMAL0;
	float3 tangent: TANGENT0;
	float3 binormal: BINORMAL0;
	float2 texCoord: TEXCOORD0;	
};

struct Pixel_Svp4T2
{
	float4 svPos: SV_POSITION;
	float2 texCoord: TEXCOORD0;
};

struct Pixel_Svp4P4T2
{
	float4 svPos: SV_POSITION;
	float4 pos: POSITION;
	float2 texCoord: TEXCOORD0;
};

struct Pixel_Svp4P4
{
	float4 svPos : SV_POSITION;
	float4 pos : POSITION0;
};

struct Pixel_Svp4C4
{
	float4 svPos: SV_POSITION;
	float4 color: COLOR0;
};

struct Pixel_Svp4N3
{
	float4 svPos: SV_POSITION;
	float3 normal: NORMAL0;
};

struct Pixel_Svp4N3T2
{
	float4 svPos: SV_POSITION;
	float3 normal: NORMAL0;
	float2 texCoord: TEXCOORD0;
};

struct Pixel_Svp4P3N3T2
{
	float4 svPos: SV_POSITION;
	float3 pos: POSITION0;
	float3 normal: NORMAL0;
	float2 texCoord: TEXCOORD0;
};

struct Pixel_Svp4P3N3Tg3Bn3T2
{
	float4 svPos: SV_POSITION;
	float3 pos: POSITION0;
	float3 normal: NORMAL0;
	float3 tangent: TANGENT0;
	float3 binormal: BINORMAL0;
	float2 texCoord: TEXCOORD0;
};

struct Pixel_Svp4P3N3Tg3Bn3T2Pl4
{
	float4 svPos: SV_POSITION;
	float3 pos: POSITION0;
	float3 normal: NORMAL0;
	float3 tangent: TANGENT0;
	float3 binormal: BINORMAL0;
	float2 texCoord: TEXCOORD0;
	float4 posLightSpace[8]: TEXCOORD1;
};
