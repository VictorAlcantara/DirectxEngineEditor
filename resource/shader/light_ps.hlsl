#pragma pack_matrix(row_major)

#include "common.hlsli"

Texture2D texDiffuse : register(t0);
Texture2D texSpecular : register(t1);
Texture2D texNormalMap : register(t2);
Texture2D texShadowMaps[MAX_LIGHTS] : register(t3);
SamplerState texSampler;
SamplerState shadowMapSampler;

cbuffer cbMaterial : register(b0)
{
    Material material;
};

cbuffer cbLight : register(b1)
{
    PointLight pointLights[MAX_LIGHTS];
    SpotLight spotLights[MAX_LIGHTS];
};

cbuffer cbCamera : register(b2)
{
    float3 cameraPos;
    float padding;
};

float4 main(Pixel_Svp4P3N3Tg3Bn3T2Pl4 input) : SV_TARGET
{
    float4 result = float4( 0.0f, 0.0f, 0.0f, 0.0f );
    float3 normalMapColor = texNormalMap.Sample( texSampler, input.texCoord ).rgb;
    float3 normal;

    if ( normalMapColor.r < 0.001f || normalMapColor.g < 0.001f || normalMapColor.b < 0.001f )
        normal = normalize(input.normal);
    else
    {
        float3x3 normalTransform = float3x3( normalize(input.binormal), normalize(input.tangent), normalize(input.normal) );
        normal = (normalMapColor.rgb * 2.0f) - 1.0f;
        normal = mul( normal, normalTransform );
        normal = normalize(normal);
    }

    PointLightParam pointParam;
    pointParam.pos = input.pos;
    pointParam.normal = normal;
    pointParam.texCoord = input.texCoord;
    pointParam.cameraPos = cameraPos;
    pointParam.texDiffuse = texDiffuse;
    pointParam.texSpecular = texSpecular;
    pointParam.texSampler = texSampler;

    SpotLightParam spotParam;
    spotParam.pos = input.pos;
    spotParam.texCoord = input.texCoord;
    spotParam.normal = normal;
    spotParam.cameraPos = cameraPos;
    spotParam.texDiffuse = texDiffuse;
    spotParam.texSpecular = texSpecular;
    spotParam.samplerTex = texSampler;
    spotParam.samplerShadowMap = shadowMapSampler;

    [unroll] for ( uint i = 0; i < MAX_LIGHTS; i++ )
    {
        pointParam.light = pointLights[i];
        result += calculatePointLight( pointParam, material );

        spotParam.light = spotLights[i];
        spotParam.texShadowMap = texShadowMaps[i];
        spotParam.posLightSpace = input.posLightSpace[i];
        result += calculateSpotLight( spotParam, material );
    }

    return saturate(result);
}
