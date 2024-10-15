#ifndef _WATER_FX_
#define _WATER_FX_

#include "params.fx"
#include "utils.fx"

// 정점 셰이더로 전달될 입력 구조체
struct VS_IN
{
    float3 pos : POSITION; // 정점 위치
    float2 uv : TEXCOORD; // 텍스처 좌표
    float3 normal : NORMAL; // 정점 노멀
    float3 tangent : TANGENT; // 접선 벡터 (Tangent)
};

// 정점 셰이더에서 픽셀 셰이더로 전달되는 출력 구조체
struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
    float3 viewDir : TEXCOORD1;
    float2 texCoord : TEXCOORD2;
};

// 정점 셰이더: 정점을 변환하고 필요한 데이터를 픽셀 셰이더로 전달
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;

    float4 worldPos = mul(float4(input.pos, 1.0), g_matWorld);
    float4 viewPos = mul(worldPos, g_matView);
    output.pos = mul(viewPos, g_matProjection);

    output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWorld).xyz);
    output.uv = worldPos.xyz;
    output.viewDir = normalize(g_matViewInv[3].xyz - worldPos.xyz);
    output.texCoord = input.uv;

    return output;
}

// 픽셀 셰이더: 물의 기본 색상(파란색)을 설정하고 반사, 굴절, 노멀맵 효과를 추가
float4 PS_Main(VS_OUT input) : SV_TARGET
{
    // 텍스처 샘플링을 위한 기본 설정
    float3 normal = normalize(input.viewNormal);
    float3 viewDir = normalize(input.viewDir);
    float3 baseColor = float3(0.0f, 0.1f, 0.2f);
    
    // **0. 물의 흐름
    float time = g_float_0; // 시간 값
    float2 scrollSpeed = float2(0.1, 0.05); // 물 흐름의 속도 (x, y 방향)

    float2 scrolledTexCoord = input.texCoord + scrollSpeed * time; // UV 좌표 이동

    
    // **1. 노멀 맵 적용**
    float3 normalMapSample = g_textures[0].Sample(g_sam_0, scrolledTexCoord).rgb; // 노멀 맵 샘플링
    normalMapSample = normalMapSample * 2.0 - 1.0; // [0, 1] 범위를 [-1, 1]로 변환
    
    // **2. 큐브 맵 반사 적용**
    float3 reflectedDir = reflect(viewDir, normalMapSample);
    float3 cubeReflection = g_texCube.Sample(g_sam_0, reflectedDir).rgb;

    // **3. 실시간 반사(Real-Time Reflection) 적용**
    float3 reflectionColor = g_textures[1].Sample(g_sam_0, input.texCoord).rgb;

    // **4. 굴절 계산**
    float3 refractedDir = refract(viewDir, normal, 1.0 / 1.33); // 물 굴절률 1.33 적용
    float3 refractionColor = g_refractionTex.Sample(g_sam_0, input.texCoord).rgb;

    // **5. Fresnel 효과**
    float fresnelFactor = pow(1.0 - saturate(dot(viewDir, normal)), 3.0);

    // **6. 최종 반사 색상 합성**
    float3 finalReflection = lerp(cubeReflection, reflectionColor, fresnelFactor); // Fresnel 비율에 따라 반사 조합

    // 굴절과의 최종 합성
    //float3 finalColor = lerp(refractionColor, finalReflection, fresnelFactor);
    float3 finalColor = lerp(cubeReflection, baseColor, 0.5f);

    return float4(reflectionColor, 1.0f);
}

#endif