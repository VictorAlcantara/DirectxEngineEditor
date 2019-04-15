#pragma pack_matrix(row_major)

cbuffer PerObject
{
    float4 color;
};

float4 main() : SV_TARGET
{
    return color;
}