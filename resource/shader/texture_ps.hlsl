#pragma pack_matrix(row_major)

Texture2D tex0;
SamplerState samp0;

cbuffer cbColor
{
    float4 color;
};

float4 main(float4 pos: SV_POSITION, float2 texCoord: TEXCOORD) : SV_TARGET
{
    float4 texColor = tex0.Sample(samp0, texCoord);
    return color * texColor;
}
