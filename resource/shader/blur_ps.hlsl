#pragma pack_matrix(row_major)

#include "common.hlsli"

Texture2D tex;
SamplerState sampState;

cbuffer BlurData
{
    float2 blurDir;
    float2 blurDisplace;
    MERGE(float, BLUR_WEIGHTS_FLOAT_COUNT) blurWeights[BLUR_WEIGHTS_ARRAY_SIZE];
    uint blurWeightCount;
    float blurBaseTextureWeight;
    float2 targetDimension;
};

static float weights[BLUR_WEIGHTS_ARRAY_SIZE * BLUR_WEIGHTS_FLOAT_COUNT] = (float[BLUR_WEIGHTS_ARRAY_SIZE * BLUR_WEIGHTS_FLOAT_COUNT])blurWeights;

float4 main(Pixel_Svp4T2 input) : SV_TARGET
{
    float4 texColor = tex.Sample( sampState, input.texCoord );
    texColor.a = 1.0f;

    float2 pixelSize = 1.0f / targetDimension;
    float baseWeight = blurBaseTextureWeight;
    float4 blurColor = texColor * baseWeight;
    baseWeight = (1.0f - baseWeight) * 0.5f;

    for ( int sign = -1; sign <= 1; sign += 2 )
    {
        float fSign = (float)sign;
        for ( uint i = 0; i < blurWeightCount; i++ )
        {
            float texDisplaceFactor = (float)(i+1);
            float2 texDisplace = blurDisplace * texDisplaceFactor * pixelSize * blurDir * fSign;
            blurColor += tex.Sample( sampState, input.texCoord + texDisplace ) * weights[i] * baseWeight;
        }
    }    

    return blurColor;
}
