#ifndef _WATER_FX_
#define _WATER_FX_

#include "params.fx"

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
    float3 viewTangent : TANGENT;
    float3 viewBinormal : BINORMAL;
};

// 정점 셰이더: 정점을 변환하고 필요한 데이터를 픽셀 셰이더로 전달
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;

    output.viewPos = mul(float4(input.pos, 1.f), g_matWV).xyz;
    output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);
    output.viewTangent = normalize(mul(float4(input.tangent, 0.f), g_matWV).xyz);
    output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));

    return output;
}

// 픽셀 셰이더: 물의 기본 색상(파란색)을 설정하고 반사, 굴절, 노멀맵 효과를 추가
float4 PS_Main(VS_OUT input) : SV_TARGET
{
    // 기본 물 색상 - 파란색
    float4 baseColor = float4(0.0, 0.5, 1.0, 1.0); // 파란색 기본 색상

    // 기본 노멀을 사용
    float3 perturbedNormal = normalize(input.viewNormal);

    // 반사 벡터 계산
    float3 viewDir = normalize(g_matViewInv[3].xyz - input.viewPos);
    float3 reflectionVector = reflect(viewDir, perturbedNormal);

    // 굴절 벡터 계산 (노멀에 기반하여 굴절)
    float3 refractionVector = refract(viewDir, perturbedNormal, 0.97); // 물의 굴절률(약 0.97)

    // 반사 텍스처 샘플링 (큐브 맵을 이용한 환경 맵핑)
    float4 reflectionColor = g_texCube.Sample(g_sam_0, reflectionVector);

    // 굴절 텍스처 샘플링 (물 표면의 왜곡을 표현)
    float4 refractionColor = g_refractionTex.Sample(g_sam_0, input.uv); // 노멀맵 사용하지 않음

    // 반사와 굴절 색상 혼합
    float4 finalColor = lerp(refractionColor, reflectionColor, 0.5);

    // 기본 물 색상에 반사와 굴절을 혼합하여 최종 색상 계산
    finalColor = lerp(baseColor, reflectionColor, 0.5);

    finalColor.a = 0.5;

    return finalColor;
}

#endif