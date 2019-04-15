#pragma pack_matrix(row_major)

#include "common.hlsli"

cbuffer cbTransform : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
    matrix worldInvTransp;
};

cbuffer cbTextureTransform : register(b1)
{
    matrix textureTransform;
};

cbuffer cbLightTransform : register(b2)
{
    matrix lightViews[MAX_LIGHTS];
    matrix lightProjs[MAX_LIGHTS];
};

Pixel_Svp4P3N3Tg3Bn3T2Pl4 main(Input_P3N3Tg3Bn3T2 input)
{
    Pixel_Svp4P3N3Tg3Bn3T2Pl4 result;

    result.svPos = calculateWvp( input.pos, world, view, proj );
    result.pos = mul( float4(input.pos, 1.0f), world ).xyz;
    result.texCoord = calculateTexTransform( input.texCoord, textureTransform );
    result.normal = mul( float4(input.normal, 0.0f), worldInvTransp ).xyz;
    result.tangent = mul( float4(input.tangent, 0.0f), worldInvTransp ).xyz;
    result.binormal = mul( float4(input.binormal, 0.0f), worldInvTransp ).xyz;

    [unroll] for ( uint i = 0; i < MAX_LIGHTS; i++ )
        result.posLightSpace[i] = calculateWvp( input.pos, world, lightViews[i], lightProjs[i] );

    return result;
}
