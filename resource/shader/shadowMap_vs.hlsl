#pragma pack_matrix(row_major)

#include "common.hlsli"

cbuffer cbModel
{
    matrix world;
};

cbuffer cbLightTransform
{
    matrix lightView;
    matrix lightProj;
};

Pixel_Svp4P4 main(float3 pos : POSITION0)
{
    Pixel_Svp4P4 result;

    result.pos = calculateWvp( pos, world, lightView, lightProj );
    result.svPos = result.pos;
    
    return result;
}
