#ifndef _SKYBOX_FX_
#define _SKYBOX_FX_

#include "params.fx"

// Input 구조체
struct VS_IN
{
    float3 localPos : POSITION; // 정점의 로컬 좌표 (큐브의 좌표)
};

// Output 구조체
struct VS_OUT
{
    float4 pos : SV_Position; // 클립 공간의 좌표
    float3 uvw : TEXCOORD; // 큐브 좌표계를 셰이더로 전달
};

// Vertex Shader
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    // Translation은 하지 않고 Rotation만 적용한다
    float4 viewPos = mul(float4(input.localPos, 0), g_matView); // 카메라 좌표계로 변환
    float4 clipSpacePos = mul(viewPos, g_matProjection); // 클립 공간으로 변환

    // w/w = 1이기 때문에 항상 깊이가 1로 유지된다
    output.pos = clipSpacePos.xyww; // 깊이 1로 고정
    output.uvw = input.localPos; // 큐브의 로컬 좌표를 그대로 넘겨줌 (큐브맵 샘플링에 사용)

    return output;
}

// Pixel Shader
float4 PS_Main(VS_OUT input) : SV_Target
{
    // 큐브맵 텍스처에서 샘플링
    float4 color = g_texCube.Sample(g_sam_0, input.uvw);

    return color;
}

#endif
