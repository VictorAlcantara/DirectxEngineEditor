#pragma pack_matrix(row_major)

#include "common.hlsli"

float3 main(Pixel_Svp4P4 input) : SV_TARGET
{
    float depth = input.pos.z / input.pos.w;

    return float3( depth, depth, depth );
}
