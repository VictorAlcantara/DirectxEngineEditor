#pragma pack_matrix(row_major)

#include "common.hlsli"

cbuffer cbWvp
{
    matrix world;
    matrix view;
    matrix proj;
    matrix worldInvTransp;
};

cbuffer cbTextureTransform
{
    matrix texTransform;
};

Pixel_Svp4N3T2 main(Input_P3N3T2 input)
{
    Pixel_Svp4N3T2 result;

    result.svPos = calculateWvp( input.pos, world, view, proj );
    result.normal = mul3(input.normal, worldInvTransp);
    result.texCoord = calculateTexTransform( input.texCoord, texTransform );

    return result;
}
