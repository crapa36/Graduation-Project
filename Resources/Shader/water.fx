#ifndef _WATER_FX_
#define _WATER_FX_

#include "params.fx"  // params.fx 파일을 포함

// 버텍스 셰이더 입력 구조체
struct VS_IN
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
};

// 버텍스 셰이더 출력 구조체
struct VS_OUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
    float3 Normal : TEXCOORD2;
};

// 버텍스 셰이더
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;

    // 월드 좌표에서의 위치와 노멀 계산
    float4 worldPos = mul(float4(input.Pos, 1.0f), g_matWorld);
    output.WorldPos = worldPos.xyz;

    float4 viewPos = mul(worldPos, g_matView);
    output.Pos = mul(viewPos, g_matProjection);

    output.Normal = normalize(mul(float4(input.Normal, 0.0f), g_matWorld).xyz);
    output.TexCoord = input.TexCoord;

    return output;
}

// 픽셀 셰이더 입력 구조체
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
    float3 Normal : TEXCOORD2;
};

// 물 픽셀 셰이더
float4 PS_Main(PS_INPUT input) : SV_TARGET
{
    // 뷰 방향과 반사 벡터 계산
    float3 viewDir = normalize(input.WorldPos - g_matViewInv[3].xyz);
    float3 reflectDir = reflect(viewDir, input.Normal);

    // 반사 텍스처 샘플링
    float4 reflection = g_reflectionTex.Sample(g_sam_0, reflectDir.xy * 0.5 + 0.5);
    // 굴절 텍스처 샘플링
    float4 refraction = g_refractionTex.Sample(g_sam_0, input.TexCoord);

    // 물의 투명도와 반사, 굴절을 조합
    float4 waterColor = lerp(refraction, reflection, 0.4); // 반사와 굴절을 혼합
    waterColor.a = 0.5; // 알파 값 설정 (투명도)

    return waterColor;
}
#endif