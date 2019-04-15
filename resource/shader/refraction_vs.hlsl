#pragma pack_matrix(row_major)

#include "common.hlsli"

cbuffer cbWvp
{
    matrix world;
	matrix view;
	matrix proj;
};

cbuffer cbTextureTransform
{
	matrix texTransf;
}

Pixel_Svp4P4T2 main(Input_P3N3Tg3Bn3T2 input)
{
    Pixel_Svp4P4T2 result;

    result.svPos = calculateWvp( input.pos, world, view, proj );
    result.pos = result.svPos;
    result.texCoord = calculateTexTransform(input.texCoord, texTransf);

    return result;
}
