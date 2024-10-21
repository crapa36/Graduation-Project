// water_vs.hlsl
#include "params.fx"

struct VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
    float3 tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float3 worldPos : TEXCOORD;
    float3 normal : TEXCOORD1;
    float2 texCoord : TEXCOORD2;
    float3 tangent : TEXCOORD3;
    float3 viewDir : TEXCOORD4;
};

VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output;

    // 월드 공간으로 변환
    float4 worldPos = mul(float4(input.pos, 1.0f), g_matWorld);
    output.worldPos = worldPos.xyz;

    // 뷰 방향 계산
    float3 viewPosition = mul(float4(0, 0, 0, 1), g_matViewInv).xyz; // 카메라 위치
    output.viewDir = normalize(viewPosition - output.worldPos);

    // 정점 위치 변환
    output.pos = mul(worldPos, g_matView);
    output.pos = mul(output.pos, g_matProjection);

    // 노멀과 탄젠트 변환
    output.normal = mul(float4(input.normal, 0.0f), g_matWorld).xyz;
    output.tangent = mul(float4(input.tangent, 0.0f), g_matWorld).xyz;

    // 텍스처 좌표 전달
    output.texCoord = input.texCoord;

    return output;
}


// PBR 구조체
struct PBRMaterial
{
    float3 albedo;
    float metallic;
    float roughness;
    float3 normalMap;
};

// 함수: 노멀 벡터 변환 (TBN 매트릭스 사용)
float3 TransformNormal(float3 normal, float3 tangent, float3 bitangent)
{
    float3x3 TBN = float3x3(normalize(tangent), normalize(bitangent), normalize(normal));
    return normalize(mul(normal, TBN));
}

// Fresnel 함수
float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

// 메인 Pixel Shader
float4 PS_Main(VS_OUTPUT input) : SV_Target
{
    // 노멀 맵 샘플링
    float3 normal = normalize(input.normal);
    // 노멀 맵을 적용하려면 추가 코드 필요 (예: tangent space에서 world space로 변환)

    // 기본 PBR 파라미터 (예시 값)
    PBRMaterial material;
    material.albedo = float3(0.0f, 0.3f, 0.5f); // 물의 기본 색상
    material.metallic = 0.0f;
    material.roughness = 0.1f;
    material.normalMap = normal;

    // 기본 조명 계산
    float3 N = normalize(normal);
    float3 V = normalize(input.viewDir);

    // 환경 맵을 사용한 반사
    float3 R = reflect(-V, N);
    float3 reflection = g_texCube.Sample(g_sam_0, R).rgb;

    // 굴절 계산
    float3 refraction = g_refractionTex.Sample(g_sam_0, input.texCoord).rgb;

    // Fresnel 효과
    float3 F0 = float3(0.04f, 0.04f, 0.04f); // 비금속 재질의 기본 F0
    float3 F = FresnelSchlick(max(dot(N, V), 0.0f), F0);

    // 최종 색상 계산
    float3 color = lerp(refraction, reflection, F);

    // 간단한 라이팅 (추가적인 PBR 라이팅 모델을 적용할 수 있음)
    for (int i = 0; i < g_lightCount; ++i)
    {
        LightInfo light = g_light[i];
        float3 L = normalize(light.position.xyz - input.worldPos);
        float3 H = normalize(L + V);
        float NdotL = max(dot(N, L), 0.0f);
        float NdotH = max(dot(N, H), 0.0f);
        float NdotV = max(dot(N, V), 0.0f);
        float VdotH = max(dot(V, H), 0.0f);

        // 거울 반사 항
        float alpha = pow(1.0f - material.roughness, 2.0f);
        float D = (alpha * alpha) / (3.14159265359f * pow((NdotH * NdotH) * (alpha * alpha - 1.0f) + 1.0f, 2.0f));

        // 기하학적 감쇠 항 (Schlick-GGX)
        float k = (alpha + 1.0f) * (alpha + 1.0f) / 8.0f;
        float G = min(1.0f, min((2.0f * NdotH * NdotV) / VdotH, (2.0f * NdotH * NdotL) / VdotH));

        // Fresnel 항 (이미 계산됨)
        // float3 F = FresnelSchlick(max(dot(N, V), 0.0f), F0);

        // 최종 BRDF
        float3 specular = (D * F * G) / (4.0f * NdotV * NdotL + 0.001f);
        float3 diffuse = material.albedo / 3.14159265359f;

        color += (diffuse * light.color.ambient + (diffuse + specular) * light.color.diffuse) * NdotL;
    }

    // 최종 색상 클램핑
    color = saturate(color);

    return float4(color, 1.0f);
}