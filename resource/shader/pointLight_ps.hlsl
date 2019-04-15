#pragma pack_matrix(row_major)

#include "common.hlsli"

Texture2D texDiffuse : register(t0);
Texture2D texSpecular : register(t1);
Texture2D texNormalMap : register(t2);
SamplerState texSampler;

cbuffer cbMaterial : register(b0)
{
    Material material;
};

cbuffer cbPointLightArray : register(b1)
{
    PointLight pointLight;
};

cbuffer cbCamera : register(b2)
{
    float3 cameraPos;
    float padding;
};

float4 main(Pixel_Svp4P3N3Tg3Bn3T2 input) : SV_TARGET
{
    float3 normalMapColor = texNormalMap.Sample( texSampler, input.texCoord ).rgb;
    float3 normal;

    if ( normalMapColor.r < 0.001f && normalMapColor.g < 0.001f && normalMapColor.b < 0.001f )
        normal = normalize(input.normal);
    else
    {
        float3x3 normalTransform = float3x3( normalize(input.binormal), normalize(input.tangent), normalize(input.normal) );
        normal = (normalMapColor.rgb * 2.0f) - 1.0f;
        normal = mul( normal, normalTransform );
    }

    float4 result = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 textureColor = texDiffuse.Sample( texSampler, input.texCoord );
    float3 incident = input.pos - pointLight.pos;
    float incidentDist = length( incident );
    float3 incidentNorm = incident / incidentDist;
    float diffuseIntensity = saturate( dot(-incidentNorm, normal) );

    if ( incidentDist > pointLight.range )
        return result;

    float4 diffuseColor = material.diffuseColor * pointLight.diffuseColor;
    float4 diffuse = diffuseColor * diffuseIntensity;

    float3 pointToCam = cameraPos - input.pos;
    float3 halfVector = normalize(pointToCam + -incident);
    float specularIntensity = dot( halfVector, normal );
    float4 specularColorTex = texSpecular.Sample( texSampler, input.texCoord ).rgba;
    specularIntensity = pow(specularIntensity, material.specularPower * specularColorTex.a);

    float4 specular = material.specularColor * float4(specularColorTex.rgb, 1.0f) * pointLight.specularColor * specularIntensity * diffuseIntensity;

    float attenuation = calculateAttenuation( pointLight.range, incidentDist, pointLight.atten0, pointLight.atten1, pointLight.atten2 );

    result = (diffuse + specular) * attenuation;
    result *= textureColor;

    return saturate(result);
}
