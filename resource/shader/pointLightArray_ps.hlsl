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
    PointLight pointLightArray[MAX_LIGHTS];
};

cbuffer cbCamera : register(b2)
{
    float3 cameraPos;
    float padding;
};

float4 calculateLight(PointLight pointLight, Pixel_Svp4P3N3Tg3Bn3T2 input, float3 normal)
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if ( pointLight.range < 0.1f )
        return color;

    float4 textureColor = texDiffuse.Sample( texSampler, input.texCoord );
    float3 incident = input.pos - pointLight.pos;
    float incidentDist = length( incident );
    float3 incidentNorm = incident / incidentDist;
    float diffuseIntensity = saturate( dot(-incidentNorm, normal) );

    if ( incidentDist > pointLight.range )
        return color;

    float4 diffuseColor = material.diffuseColor * pointLight.diffuseColor;
    float4 diffuse = diffuseColor * diffuseIntensity;

    float3 pointToCam = cameraPos - input.pos;
    float3 halfVector = normalize(pointToCam + -incident);
    float4 specularColorTex = texSpecular.Sample( texSampler, input.texCoord ).rgba;
    float specularIntensity = pow(saturate(dot(halfVector, normal)), material.specularPower * specularColorTex.a);

    float4 specular = material.specularColor * float4(specularColorTex.rgb, 1.0f) * pointLight.specularColor * specularIntensity * diffuseIntensity;

    float attenuation = calculateAttenuation( pointLight.range, incidentDist, pointLight.atten0, pointLight.atten1, pointLight.atten2 );

    color = (diffuse + specular) * attenuation;
    color *= textureColor;

    return color;
}

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
    
    [unroll] for ( uint i = 0; i < MAX_LIGHTS; i++ )
        result += calculateLight( pointLightArray[i], input, normal );

    result = saturate(result);
    return result;
}
