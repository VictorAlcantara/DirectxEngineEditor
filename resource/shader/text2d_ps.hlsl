#pragma pack_matrix(row_major)

#include "common.hlsli"

Texture2D tex0;
SamplerState samp0;

cbuffer cbMaterial
{
    float4 color;
};

float4 main(Pixel_Svp4T2 input) : SV_TARGET
{
    float4 texColor = tex0.Sample( samp0, input.texCoord );
    float4 result = texColor;

    result.a = (texColor.r + texColor.g + texColor.b) / 3.0f;
    result *= color;

    return result;
}