#ifndef _UTILS_FX_
#define _UTILS_FX_

#include "params.fx"

// Blinn-Phong ���� ����� ������ �ݻ� ���
float CalculateSpecular(float3 lightDir, float3 viewNormal, float3 viewPos, float3 lightColor, float shininess)
{
    float3 viewDir = normalize(viewPos);
    float3 halfDir = normalize(lightDir + viewDir);
    float specAngle = saturate(dot(viewNormal, halfDir));
    return pow(specAngle, shininess);
}

LightColor CalculateLightColor(int lightIndex, float3 viewNormal, float3 viewPos)
{
    LightColor color = (LightColor) 0.f;

    float3 viewLightDir = (float3) 0.f;

    float diffuseRatio = 0.f;
    float specularRatio = 0.f;
    float distanceRatio = 1.f;

    if (g_light[lightIndex].lightType == 0)
    {
        // Directional Light
        viewLightDir = normalize(mul(float4(g_light[lightIndex].direction.xyz, 0.f), g_matView).xyz);
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal));
    }
    else if (g_light[lightIndex].lightType == 1)
    {
        // Point Light
        float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;
        viewLightDir = normalize(viewPos - viewLightPos);
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal));

        float dist = distance(viewPos, viewLightPos);
        if (g_light[lightIndex].range == 0.f)
            distanceRatio = 0.f;
        else
            distanceRatio = saturate(1.f - pow(dist / g_light[lightIndex].range, 2));
    }
    else
    {
        // Spot Light
        float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;
        viewLightDir = normalize(viewPos - viewLightPos);
        diffuseRatio = saturate(dot(-viewLightDir, viewNormal));

        if (g_light[lightIndex].range == 0.f)
            distanceRatio = 0.f;
        else
        {
            float halfAngle = g_light[lightIndex].angle / 2;

            float3 viewLightVec = viewPos - viewLightPos;
            float3 viewCenterLightDir = normalize(mul(float4(g_light[lightIndex].direction.xyz, 0.f), g_matView).xyz);

            float centerDist = dot(viewLightVec, viewCenterLightDir);
            distanceRatio = saturate(1.f - centerDist / g_light[lightIndex].range);

            float lightAngle = acos(dot(normalize(viewLightVec), viewCenterLightDir));

            if (centerDist < 0.f || centerDist > g_light[lightIndex].range) // �ִ� �Ÿ��� �������
                distanceRatio = 0.f;
            else if (lightAngle > halfAngle) // �ִ� �þ߰��� �������
                distanceRatio = 0.f;
            else // �Ÿ��� ���� ������ ���⸦ ����
                distanceRatio = saturate(1.f - pow(centerDist / g_light[lightIndex].range, 2));
        }
    }

    // Blinn-Phong ������ �ݻ� ���
    specularRatio = CalculateSpecular(viewLightDir, viewNormal, viewPos, g_light[lightIndex].color.specular, 32.0); // shininess�� 32�� ����

    color.diffuse = g_light[lightIndex].color.diffuse * diffuseRatio * distanceRatio;
    color.ambient = g_light[lightIndex].color.ambient * distanceRatio;
    color.specular = g_light[lightIndex].color.specular * specularRatio * distanceRatio;

    return color;
 }



float Rand(float2 co) // [0.0~1.0]
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float CalculateTesslationLevel(float3 cameraWorldPos, float3 patchPos, float min, float max, float maxLv)
{
    float dist = distance(cameraWorldPos, patchPos);
    if (dist< min)
        return maxLv;
    if (dist > max)
        return 1.0f;
    float level = (maxLv - 1.f) * (1.f - (dist - min) / (max - min));
    //float level = lerp(maxLv, 1.f, saturate((dist - min) / (max - min)));
    return level;
}
struct SkinningInfo
{
    float3 pos;
    float3 normal;
    float3 tangent;
};

void Skinning(inout float3 pos, inout float3 normal, inout float3 tangent,
    inout float4 weight, inout float4 indices)
{
    SkinningInfo info = (SkinningInfo) 0.f;

    for (int i = 0; i < 4; ++i)
    {
        if (weight[i] == 0.f)
            continue;

        int boneIdx = indices[i];
        matrix matBone = g_mat_bone[boneIdx];

        info.pos += (mul(float4(pos, 1.f), matBone) * weight[i]).xyz;
        info.normal += (mul(float4(normal, 0.f), matBone) * weight[i]).xyz;
        info.tangent += (mul(float4(tangent, 0.f), matBone) * weight[i]).xyz;
    }

    pos = info.pos;
    tangent = normalize(info.tangent);
    normal = normalize(info.normal);
}

// ���� ���� �̿��Ͽ� ��ũ�� ��ǥ�� ���� ��ǥ�� ��ȯ
float3 ReconstructPosition(float2 uv, float depth)
{
    // NDC ��ǥ�� ��ȯ
    float4 clipPos = float4(uv * 2.0f - 1.0f, depth, 1.0f);
    
    // ���� ��ǥ�� ��ȯ
    float4 viewPos = mul(g_matProjection, clipPos);
    viewPos /= viewPos.w; // ���� ��ǥ ��ȯ
    return viewPos.xyz;
}

// ȭ�� �������� ���� Ʈ���̽�
float3 ScreenSpaceRayTracing(float2 uv, float3 rayDir, float currentDepth)
{
    float3 reflectedColor = float3(0, 0, 0);
    float2 currentUV = uv;
    
    // ������ ������ ���� �̵��ϸ鼭 ���� ���� ���ø�
    for (int i = 0; i < 50; ++i)
    {
        currentUV += rayDir.xy * 0.01; // �̵� �Ÿ�

        // ȭ�� ��踦 �Ѿ�� ��� ����
        if (currentUV.x < 0.0f || currentUV.x > 1.0f || currentUV.y < 0.0f || currentUV.y > 1.0f)
            break;

        // ���� ��ġ�� ���� �� ���ø�
        float sceneDepth = g_textures[2].Sample(g_sam_0, currentUV).r;

        // ���� ���� ���� ���� ��
        if (sceneDepth < currentDepth) // ���̸� ���ϴ� ���� ����
        {
            reflectedColor = g_textures[1].Sample(g_sam_0, currentUV).rgb;
            break;
        }
    }

    return reflectedColor;
}


// GGX ���� �Լ�
float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float numerator = a2;
    float denominator = (NdotH2 * (a2 - 1.0) + 1.0);
 
    denominator = PI * denominator * denominator;

    return numerator / denominator;
}

// Schlick�� �ٻ� ���� �Լ�
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float numerator = NdotV;
    float denominator = NdotV * (1.0 - k) + k;

    return numerator / denominator;
}

// Smith�� ���� �Լ�
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

// Fresnel-Schlick �ٻ�
float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

#endif
