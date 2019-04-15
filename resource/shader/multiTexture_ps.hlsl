#pragma pack_matrix(row_major)

#include "common.hlsli"

cbuffer cbMultiTexture
{
    float4 textureWeights[MULTI_TEXTURE_ARRAY_COUNT];
    float4 ambient;
    uint textureCount;
};

Texture2D texs[MULTI_TEXTURE_ARRAY_MAX];
SamplerState samp0;
 
static float weights[MULTI_TEXTURE_ARRAY_MAX] = (float[MULTI_TEXTURE_ARRAY_MAX])textureWeights;

float4 main(Pixel_Svp4T2 input) : SV_TARGET
{
    float4 color = float4( 0.0f, 0.0f, 0.0f, 0.0f );
    for ( uint i = 0; i < min(textureCount, MULTI_TEXTURE_ARRAY_MAX); i++ )
        color += texs[i].Sample( samp0, input.texCoord ) * weights[i];
    
    return color * ambient;
}