#pragma pack_matrix(row_major)

#include "common.hlsli"

cbuffer cbPerObject
{
    matrix world;
    matrix view;
    matrix proj;
};

cbuffer cbMatrixTransform
{
    matrix texTransf;
};

Pixel_Svp4T2 main(Input_P3T2 input)
{
    Pixel_Svp4T2 result;

    result.svPos = calculateWvp( input.pos, world, view, proj );

	result.texCoord = calculateTexTransform(input.texCoord, texTransf);

    return result;
}