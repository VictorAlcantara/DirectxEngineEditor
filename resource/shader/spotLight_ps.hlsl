#pragma pack_matrix(row_major)

#include "common.hlsli"

Texture2D texDiffuse : register(t0);
Texture2D texSpecular : register(t1);
Texture2D texNormalMap : register(t2);
Texture2D texShadowMaps[MAX_LIGHTS] : register(t3);
SamplerState texSampler;
SamplerState shadowMapSampler;

cbuffer cbMaterial : register(b0)
{
    Material material;
};

cbuffer cbSpotLight : register(b1)
{
    SpotLight spotLights[MAX_LIGHTS];
};

cbuffer cbCamera : register(b2)
{
    float3 cameraPos;
    float padding;
};

float4 calculateLight(Pixel_Svp4P3N3Tg3Bn3T2Pl4 input, uint index)
{
    float4 result = float4(0.0f, 0.0f, 0.0f, 0.0f);
    SpotLight spotLight = spotLights[index];

    if ( spotLight.outerConeRads + spotLight.innerConeRads < 0.008f )
        return result;

    float3 incident = input.pos - spotLight.pos;
    float incidentDist = length( incident );
    float3 incidentNorm = incident / incidentDist;
    float cosInnerCone = cos( spotLight.innerConeRads * 0.5f );
    float cosOuterCone = cos( (spotLight.outerConeRads + spotLight.innerConeRads) * 0.5f );
    float incidentDotDirection = dot(incidentNorm, spotLight.direction);

    if ( incidentDist > spotLight.range || incidentDotDirection < cosOuterCone )
        return result;

    // Shadow related
    float shadowAmount = 1.0f;
    if ( spotLight.shadowEnabled )
    {
        float4 posLightSpace = input.posLightSpace[index];
        posLightSpace.xyz /= posLightSpace.w;

        float distanceRatio = incidentDist / spotLight.range;

        float2 shadowMapTexCoord;
        shadowMapTexCoord.x = posLightSpace.x * 0.5f + 0.5f;
        shadowMapTexCoord.y = posLightSpace.y * -0.5f + 0.5f;

        float lightToPointDepth = posLightSpace.z;
        float shadowMapDepth = texShadowMaps[index].Sample( shadowMapSampler, shadowMapTexCoord ).r;
        shadowAmount = step( lightToPointDepth - spotLight.shadowBias * (1.0f + distanceRatio * spotLight.distanceAdjustment), shadowMapDepth );
    }

    float3 normalMapColor = texNormalMap.Sample( texSampler, input.texCoord ).rgb;
    float3 normal;

    if ( normalMapColor.r < 0.001f || normalMapColor.g < 0.001f || normalMapColor.b < 0.001f )
        normal = normalize(input.normal);
    else
    {
        float3x3 normalTransform = float3x3( normalize(input.binormal), normalize(input.tangent), normalize(input.normal) );
        normal = (normalMapColor.rgb * 2.0f) - 1.0f;
        normal = mul( normal, normalTransform );
        normal = normalize(normal);
    }

    float4 textureColor = texDiffuse.Sample( texSampler, input.texCoord );

    float diffuseIntensity = saturate( dot(-incidentNorm, normal) );

    float4 diffuseColor = material.diffuseColor * spotLight.diffuseColor;
    float4 diffuse = diffuseColor * diffuseIntensity;

    float3 pointToCam = cameraPos - input.pos;
    float3 halfVector = normalize(pointToCam + -incident);
    float4 specularColorTex = texSpecular.Sample( texSampler, input.texCoord ).rgba;
    float specularIntensity = pow(saturate(dot(halfVector, normal)), material.specularPower * specularColorTex.a);

    float4 specular = material.specularColor * float4(specularColorTex.rgb, 1.0f) * spotLight.specularColor * specularIntensity * diffuseIntensity;

    float attenuation = calculateAttenuation( spotLight.range, incidentDist, spotLight.atten0, spotLight.atten1, spotLight.atten2 );

    float coneAttenuation = 1.0f;
    if ( incidentDotDirection < cosInnerCone )
    {
        coneAttenuation = calculateAttenuation( abs(cosOuterCone - cosInnerCone), abs(incidentDotDirection - cosInnerCone), 0.0f, spotLight.atten1, spotLight.atten2 );
    }

    attenuation *= coneAttenuation;

    result = (diffuse + specular) * attenuation;
    result *= textureColor * shadowAmount;

    return result;
}

float4 main(Pixel_Svp4P3N3Tg3Bn3T2Pl4 input) : SV_TARGET
{
    float4 result = float4( 0.0f, 0.0f, 0.0f, 0.0f );

    [unroll] for ( uint i = 0; i < MAX_LIGHTS; i++ )
        result += calculateLight( input, i );

    return saturate(result);
}
