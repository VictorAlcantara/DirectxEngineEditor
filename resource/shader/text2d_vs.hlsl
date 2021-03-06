#pragma pack_matrix(row_major)

#include "common.hlsli"

cbuffer cbWvp
{
    matrix world;
    matrix view;
    matrix proj;
};

cbuffer cbTexture
{
    matrix textureTransform;
};

Pixel_Svp4T2 main(Input_P3T2 input)
{
    Pixel_Svp4T2 result;
    
    result.svPos = calculateWvp( input.pos, world, view, proj );
    result.texCoord = calculateTexTransform( input.texCoord, textureTransform );

    return result;
}

