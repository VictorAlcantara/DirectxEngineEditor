#pragma pack_matrix(row_major)

#include "common.hlsli"

cbuffer cbWvp
{
    matrix world;
    matrix view;
    matrix proj;
    matrix worldInvTransp;
};

Pixel_Svp4N3 main(Input_P3N3 input)
{
    Pixel_Svp4N3 result;

    result.svPos = calculateWvp( input.pos, world, view, proj );
    result.normal = mul3(input.normal, worldInvTransp);

    return result;
}
