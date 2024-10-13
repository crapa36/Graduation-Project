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

    output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);
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

    // **1. 노멀 맵 적용**
    //float3 normalMapSample = g_textures[0].Sample(g_sam_0, input.texCoord).rgb; // 노멀 맵 샘플링
    //normalMapSample = normalMapSample * 2.0 - 1.0; // [0, 1] 범위를 [-1, 1]로 변환
    //normal = normalize(normal + normalMapSample); // 노멀 벡터 보정

    // **2. 큐브 맵 반사 적용**
    float3 reflectedDir = reflect(viewDir, normal);
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
    float3 finalColor = lerp(refractionColor, finalReflection, fresnelFactor);

    return float4(cubeReflection, 1.0);
}

#endif