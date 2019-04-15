#pragma pack_matrix(row_major)

cbuffer PerObject
{
    matrix world;
    matrix view;
    matrix proj;
};

cbuffer DepthBias
{
	float depthBias;
	float3 padding;
};

float4 main(float4 pos : POSITION) : SV_POSITION
{
    float4 output;
    pos.w = 1.0f;

    output = mul(pos, world);
    output = mul(output, view);
    output = mul(output, proj);

	output.w += depthBias;

    return output;
}