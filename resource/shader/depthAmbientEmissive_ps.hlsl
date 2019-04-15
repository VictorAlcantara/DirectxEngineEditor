#pragma pack_matrix(row_major)

#include "common.hlsli"

Texture2D texAmbient;
SamplerState texSampler;

cbuffer cbMaterial
{
    Material material;
};

cbuffer cbAmbientLight
{
    float4 ambientLight;  
};

float4 main(Pixel_Svp4T2 input) : SV_TARGET
{
    float4 result = material.emissiveColor + material.ambientColor * ambientLight;
    return saturate( result * texAmbient.Sample( texSampler, input.texCoord ) );
}
