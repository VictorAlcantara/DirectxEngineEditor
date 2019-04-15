#pragma pack_matrix(row_major)

#include "common.hlsli"

cbuffer cbMatrix
{
	matrix world;
	matrix view;
	matrix proj;
};

Pixel_Svp4T2 main(float3 pos: POSITION)
{
	Pixel_Svp4T2 result;

	result.svPos = calculateWvp( pos, world, view, proj );
	result.texCoord = homogenousPointToTexCoord( result.pos );
	
	return result;
}
