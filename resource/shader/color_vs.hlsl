#pragma pack_matrix(row_major)

#include "common.hlsli"

cbuffer PerObject
{
	matrix world;
	matrix view;
	matrix proj;
};

Pixel_Svp4C4 main(Input_P3C4 input)
{
	Pixel_Svp4C4 output;

	matrix wvp = mtxmul(world, view, proj);
	output.svPos = mul( float4(input.pos, 1.0f), wvp );
	output.color = input.color;

	return output;
}
