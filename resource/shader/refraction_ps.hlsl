#pragma pack_matrix(row_major)

#include "common.hlsli"

Texture2D texNormal : register(t0);
Texture2D texRefract : register(t1);
SamplerState sampState;

cbuffer cbRefraction
{
    float3 color3;
    float refractionIntensity;
    float colorSaturation;
    float colorSeparation;
};

float4 main(Pixel_Svp4P4T2 input) : SV_TARGET
{
    float4 result;
    float2 screenCoord = homogenousPointToTexCoord(input.pos);
    float3 normal = texToNormalMap(texNormal.Sample(sampState, input.texCoord).xyz);
    float2 aberration = { colorSeparation, colorSeparation };

    screenCoord += normal.xy * refractionIntensity;
    screenCoord = clamp( screenCoord, 0.0f, 1.0f );
    //result.rgb = colorFade( texRefract.Sample( sampState, screenCoord ).rgb * color3, 0.0f );
    //result.rgb = texRefract.Sample(sampState, screenCoord).rgb * color3;
    result.rgb = color3;
    result.r *= texRefract.Sample(sampState, screenCoord + aberration * 1.0f).r;
    result.g *= texRefract.Sample(sampState, screenCoord + aberration * 0.0f).g;
    result.b *= texRefract.Sample(sampState, screenCoord + aberration * -1.0f).b;
    result.rgb = colorFade( result.rgb, colorSaturation );
    result.a = 1.0f;

    return result;
}
