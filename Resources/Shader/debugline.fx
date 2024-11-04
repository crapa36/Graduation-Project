#ifndef _DEBUGLINE_FX_
#define _DEBUGLINE_FX_

#include "params.fx"

// Debug 라인의 Vertex Shader 입력 구조체
struct VS_IN
{
    float3 startPos : POSITION0; // 라인의 시작점 월드 좌표
    float3 endPos : POSITION1; // 라인의 끝점 월드 좌표
    float4 color : COLOR; // 디버그 라인 색상
};

// Vertex Shader 출력 구조체
struct VS_OUT
{
    float4 pos : SV_POSITION; // 변환된 클립 공간 좌표
    float4 color : COLOR; // 라인 색상
};

// Vertex Shader
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;

    // 월드 좌표계를 화면 좌표계로 변환
    output.pos = mul(float4(input.startPos, 1.0f), g_matWVP);
    output.color = input.color; // 시작점 색상 유지

    return output;
}

// Pixel Shader
float4 PS_Main(VS_OUT input) : SV_Target
{
    // 단순하게 컬러를 반환하여 디버그 라인을 렌더링
    return input.color;
}

#endif // _DEBUGLINE_FX_
