#pragma pack_matrix(row_major)

#include "common.hlsli"

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

float4 main(Pixel_Svp4N3 input) : SV_TARGET
{
    float4 result = ambient;
    
    for ( uint i = 0; i < 4; i++ )
    {
        float intensity = saturate( dot(lightDirs[i].rgb, normalize(-input.normal)) );
        result += lightColors[i] * diffuse * intensity;
    }

    return saturate(result);
}
