#ifndef _UTILS_FX_
#define _UTILS_FX_

#include "params.fx"

// PBR 계산에 필요한 함수들
float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = 3.14159265359 * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
} 

// 개선된 CalculateLightColor 함수
LightColor CalculateLightColor(
    int lightIndex,
    float3 viewNormal,
    float3 viewPos,
    float3 albedo,
    float metallic,
    float roughness,
    float ao)
{
    LightColor color = (LightColor) 0.f;

    float3 viewLightDir = float3(0.f, 0.f, 0.f);
    float attenuation = 1.f; // 조명 감쇠
    float NdotL = 0.f;

    // 조명 유형에 따른 방향 설정 및 감쇠 계산
    if (g_light[lightIndex].lightType == 0)  // Directional Light
    {
        viewLightDir = normalize(mul(float4(g_light[lightIndex].direction.xyz, 0.f), g_matView).xyz);
        NdotL = max(dot(viewNormal, -viewLightDir), 0.0);
        // Directional Light는 감쇠 없음
    }
    else if (g_light[lightIndex].lightType == 1)  // Point Light
    {
        float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;
        float3 lightDir = viewPos - viewLightPos;
        float distance = length(lightDir);
        viewLightDir = normalize(lightDir);
        NdotL = max(dot(viewNormal, -viewLightDir), 0.0);

        if (g_light[lightIndex].range > 0.f)
        {
            attenuation = saturate(1.0 / (distance * distance)); // Inverse Square Law
            attenuation = saturate(attenuation * (g_light[lightIndex].range / 100.0)); // 범위에 맞게 스케일링
        }
        else
        {
            attenuation = 1.f;
        }
    }
    else // Spot Light
    {
        float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;
        float3 lightDir = viewPos - viewLightPos;
        float distance = length(lightDir);
        viewLightDir = normalize(lightDir);
        NdotL = max(dot(viewNormal, -viewLightDir), 0.0);

        if (g_light[lightIndex].range > 0.f)
        {
            attenuation = saturate(1.0 / (distance * distance)); // Inverse Square Law
            attenuation = saturate(attenuation * (g_light[lightIndex].range / 100.0)); // 범위에 맞게 스케일링
        }
        else
        {
            attenuation = 1.f;
        }

        // Spotlight 각도에 따른 감쇠
        float3 viewCenterLightDir = normalize(mul(float4(g_light[lightIndex].direction.xyz, 0.f), g_matView).xyz);
        float theta = acos(dot(-viewLightDir, viewCenterLightDir)); // 빛의 방향과 뷰의 방향 사이의 각도

        float outerCone = g_light[lightIndex].angle / 2.0;
        float innerCone = outerCone * 0.8; // 부드러운 전환을 위한 내부 코너

        float spotFactor = 1.0;
        if (theta > outerCone)
        {
            spotFactor = 0.0; // 스포트라이트 외부
        }
        else if (theta > innerCone)
        {
            // 내부와 외부 코너 사이에서 부드러운 전환
            spotFactor = saturate((outerCone - theta) / (outerCone - innerCone));
        }
        // 내부 코너 내에서는 spotFactor는 1.0 유지

        attenuation *= spotFactor;
    }

    
    // NdotL과 감쇠가 유효한 경우에만 조명 계산
    if (NdotL > 0.0 && attenuation > 0.0)
    {
        // 뷰 벡터 계산
        float3 V = normalize(-viewPos);
        float3 N = normalize(viewNormal);
        float3 L = normalize(viewLightDir);
        float3 H = normalize(V + L); // Halfway vector

        // PBR 필수 요소 계산
        float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic); // 금속성에 따른 F0 계산
        float3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);

        // Specular 계산
        float3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        float3 specular = numerator / denominator;

        // kS는 스펙큘러 반사율, kD는 디퓨즈 반사율
        float3 kS = F;
        float3 kD = 1.0 - kS;
        kD *= 1.0 - metallic; // 금속성 재질은 디퓨즈 반사율 없음

        // 디퓨즈 반사율 (Lambert 반사)
        float3 diffuse = kD * albedo / 3.14159265359;

        // Radiance: light color * 강도
        float3 radiance = g_light[lightIndex].color.diffuse.rgb * g_light[lightIndex].range;

        // 최종 조명 계산
        color.diffuse += float4(diffuse * radiance * NdotL * attenuation * ao, 1.f);
        color.specular += float4(specular * radiance * NdotL * attenuation, 1.f);
    }

        

    // Ambient는 일반적으로 전역 환경 조명으로 처리
    // 필요에 따라 별도로 계산하거나, 외부에서 설정
     color.ambient += float4(albedo * ao, 1.f);

    return color;
}

// Blinn-Phong 모델을 사용한 스펙터 반사 계산
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

            if (centerDist < 0.f || centerDist > g_light[lightIndex].range) // 최대 거리를 벗어났는지
                distanceRatio = 0.f;
            else if (lightAngle > halfAngle) // 최대 시야각을 벗어났는지
                distanceRatio = 0.f;
            else // 거리에 따라 적절히 세기를 조절
                distanceRatio = saturate(1.f - pow(centerDist / g_light[lightIndex].range, 2));
        }
    }

    // Blinn-Phong 스펙터 반사 계산
    specularRatio = CalculateSpecular(viewLightDir, viewNormal, viewPos, g_light[lightIndex].color.specular, 32.0); // shininess는 32로 설정

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

// 깊이 값을 이용하여 스크린 좌표를 월드 좌표로 변환
float3 ReconstructPosition(float2 uv, float depth)
{
    // NDC 좌표로 변환
    float4 clipPos = float4(uv * 2.0f - 1.0f, depth, 1.0f);
    
    // 월드 좌표로 변환
    float4 viewPos = mul(g_matProjection, clipPos);
    viewPos /= viewPos.w; // 동차 좌표 변환
    return viewPos.xyz;
}

// 화면 공간에서 레이 트레이싱
float3 ScreenSpaceRayTracing(float2 uv, float3 rayDir, float currentDepth)
{
    float3 reflectedColor = float3(0, 0, 0);
    float2 currentUV = uv;
    
    // 레이의 방향을 따라 이동하면서 깊이 버퍼 샘플링
    for (int i = 0; i < 50; ++i)
    {
        currentUV += rayDir.xy * 0.01; // 이동 거리

        // 화면 경계를 넘어가는 경우 중지
        if (currentUV.x < 0.0f || currentUV.x > 1.0f || currentUV.y < 0.0f || currentUV.y > 1.0f)
            break;

        // 현재 위치의 깊이 값 샘플링
        float sceneDepth = g_textures[2].Sample(g_sam_0, currentUV).r;

        // 깊이 값과 현재 깊이 비교
        if (sceneDepth < currentDepth) // 깊이를 비교하는 조건 수정
        {
            reflectedColor = g_textures[1].Sample(g_sam_0, currentUV).rgb;
            break;
        }
    }

    return reflectedColor;
}

#endif
