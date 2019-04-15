#pragma pack_matrix(row_major)

#include "common.hlsli"

cbuffer cbWvpInvTransp
{
    matrix world;
    matrix view;
    matrix proj;
    matrix worldInvTransp;
};

cbuffer cbTexTransform
{
    matrix texTransform;
};

Pixel_Svp4P3N3Tg3Bn3T2 main(Input_P3N3Tg3Bn3T2 input)
{
    Pixel_Svp4P3N3Tg3Bn3T2 result;

    result.svPos = calculateWvp( input.pos, world, view, proj );
    result.pos = mul( float4(input.pos, 1.0f), world ).xyz;
    result.texCoord = calculateTexTransform( input.texCoord, texTransform );
    result.normal = mul( float4(input.normal, 0.0f), worldInvTransp ).xyz;
    result.tangent = mul( float4(input.tangent, 0.0f), worldInvTransp ).xyz;
    result.binormal = mul( float4(input.binormal, 0.0f), worldInvTransp ).xyz;

    return result;
}
