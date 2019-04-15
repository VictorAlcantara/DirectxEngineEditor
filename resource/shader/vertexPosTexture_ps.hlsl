#pragma pack_matrix(row_major)

#include "common.hlsli"

Texture2D tex0;
SamplerState samp0; 

float4 main(Pixel_Svp4T2 input) : SV_TARGET
{
	return tex0.Sample( samp0, input.texCoord ) * 0.5f;
}