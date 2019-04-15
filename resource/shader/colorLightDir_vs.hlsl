#pragma pack_matrix(row_major)

cbuffer PerObject
{
    matrix world;
    matrix view;
    matrix proj;
    matrix worldInvTransp;
};

struct PixelInput
{
    float4 pos: SV_POSITION;
	float3 normal : NORMAL0;
};

PixelInput main(float3 pos : POSITION0, float3 normal : NORMAL0)
{
    PixelInput result;

    float4 pos4 = float4(pos, 1.0f);
    result.pos = mul(pos4, world);
    result.pos = mul(result.pos, view);
    result.pos = mul(result.pos, proj);
    result.normal = mul(float4(normal, 0.0f), worldInvTransp).xyz;

    return result;
}