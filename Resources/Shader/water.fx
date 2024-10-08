#ifndef _WATER_FX_
#define _WATER_FX_

#include "params.fx"

// 정점 셰이더로 전달될 입력 구조체
struct VS_INPUT
{
    float3 pos : POSITION; // 정점 위치
    float3 normal : NORMAL; // 정점 노멀
    float2 uv : TEXCOORD0; // 텍스처 좌표
    float4 tangent : TANGENT; // 접선 벡터 (Tangent)
};

// 정점 셰이더에서 픽셀 셰이더로 전달되는 출력 구조체
struct VS_OUTPUT
{
    float4 pos : SV_POSITION; // 클립 공간에서의 위치
    float2 uv : TEXCOORD0; // 텍스처 좌표
    float3 normal : NORMAL; // 월드 공간에서의 노멀
    float3 tangent : TANGENT; // 월드 공간에서의 접선 벡터
    float3 binormal : BINORMAL; // 월드 공간에서의 바이노멀 벡터
    float3 worldPos : TEXCOORD1; // 월드 공간에서의 위치
};

// 정점 셰이더: 정점을 변환하고 필요한 데이터를 픽셀 셰이더로 전달
VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output;

    // 월드 공간으로 정점 위치 변환
    float4 worldPos = mul(float4(input.pos, 1.0f), g_matWorld);
    output.worldPos = worldPos.xyz;

    // 접선과 바이노멀 계산
    float3 worldNormal = mul(input.normal, (float3x3) g_matWorld);
    float3 worldTangent = mul(input.tangent.xyz, (float3x3) g_matWorld);
    float3 worldBinormal = cross(worldNormal, worldTangent) * input.tangent.w;

    output.normal = worldNormal;
    output.tangent = worldTangent;
    output.binormal = worldBinormal;

    // 정점 위치를 클립 공간으로 변환
    output.pos = mul(worldPos, g_matWVP);

    // 텍스처 좌표 전달
    output.uv = input.uv;

    return output;
}

// 픽셀 셰이더: 물의 기본 색상(파란색)을 설정하고 반사, 굴절, 노멀맵 효과를 추가
float4 PS_Main(VS_OUTPUT input) : SV_TARGET
{
    // 기본 물 색상 - 파란색
    float4 baseColor = float4(0.0, 0.5, 1.0, 1.0); // 파란색 기본 색상

    // 노멀맵을 샘플링 (노멀을 변경)
    float3 normalMap = g_tex_0.Sample(g_sam_0, input.uv).rgb;
    normalMap = normalize(normalMap * 2.0f - 1.0f); // [0,1] 범위에서 [-1,1]로 변환

    // 노멀을 월드 공간으로 변환
    float3 perturbedNormal = normalize(normalMap.x * input.tangent + normalMap.y * input.binormal + normalMap.z * input.normal);

    // 반사 벡터 계산
    float3 viewDir = normalize(g_matViewInv[3].xyz - input.worldPos);
    float3 reflectionVector = reflect(viewDir, perturbedNormal);

    // 굴절 벡터 계산 (노멀에 기반하여 굴절)
    float3 refractionVector = refract(viewDir, perturbedNormal, 0.97); // 물의 굴절률(약 0.97)

    // 반사 텍스처 샘플링 (큐브 맵을 이용한 환경 맵핑)
    float4 reflectionColor = g_texCube.Sample(g_sam_0, reflectionVector);

    // 굴절 텍스처 샘플링 (물 표면의 왜곡을 표현)
    float4 refractionColor = g_refractionTex.Sample(g_sam_0, input.uv + normalMap.xy * 0.1);

    // 반사와 굴절 색상 혼합
    float4 finalColor = lerp(refractionColor, reflectionColor, 0.5);

    // 기본 물 색상에 반사와 굴절을 혼합하여 최종 색상 계산
    finalColor = lerp(baseColor, finalColor, 0.5);

    return finalColor;
}

#endif