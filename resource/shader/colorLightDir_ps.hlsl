#pragma pack_matrix(row_major)

cbuffer PerObject
{
    float4 ambient;
    float4 diffuse;
    float3 lightDir;
    float padding;
};

float4 main(float4 pos: SV_POSITION, float3 normal: NORMAL0) : SV_TARGET
{
    float4 result;
    float intensity = saturate(dot(-lightDir, normalize(normal)));
    result = saturate(ambient + intensity * diffuse);
    return result;
}