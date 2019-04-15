#pragma pack_matrix(row_major)

#include "common.hlsli"

Texture2D tex0;
SamplerState samp0;

cbuffer cbLightDir4
{
    float4 lightDirs[4];
    float4 lightColors[4];
};

cbuffer cbMaterial
{
    float4 ambient;
    float4 diffuse;
};

float4 main(Pixel_Svp4N3T2 input) : SV_TARGET
{
    float4 result = ambient;
    float4 textureColor = tex0.Sample(samp0, input.texCoord);
    
    for ( uint i = 0; i < 4; i++ )
    {
        float intensity = saturate( dot(lightDirs[i].rgb, normalize(-input.normal)) );
        result += lightColors[i] * diffuse * intensity;
    }

    return saturate(result) * textureColor;
}
