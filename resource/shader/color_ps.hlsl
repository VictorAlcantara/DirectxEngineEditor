#pragma pack_matrix(row_major)

#include "common.hlsli"

cbuffer cbColor
{
	float4 color;
};

float4 main(Pixel_Svp4C4 input) : SV_TARGET
{
	return input.color * color;
}
