#ifndef _SKYBOX_FX_
#define _SKYBOX_FX_

#include "params.fx"

// Input ����ü
struct VS_IN
{
    float3 localPos : POSITION; // ������ ���� ��ǥ (ť���� ��ǥ)
};

// Output ����ü
struct VS_OUT
{
    float4 pos : SV_Position; // Ŭ�� ������ ��ǥ
    float3 uvw : TEXCOORD; // ť�� ��ǥ�踦 ���̴��� ����
};

// Vertex Shader
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    // Translation�� ���� �ʰ� Rotation�� �����Ѵ�
    float4 viewPos = mul(float4(input.localPos, 0), g_matView); // ī�޶� ��ǥ��� ��ȯ
    float4 clipSpacePos = mul(viewPos, g_matProjection); // Ŭ�� �������� ��ȯ

    // w/w = 1�̱� ������ �׻� ���̰� 1�� �����ȴ�
    output.pos = clipSpacePos.xyww; // ���� 1�� ����
    output.uvw = input.localPos; // ť���� ���� ��ǥ�� �״�� �Ѱ��� (ť��� ���ø��� ���)

    return output;
}

// Pixel Shader
float4 PS_Main(VS_OUT input) : SV_Target
{
    // ť��� �ؽ�ó���� ���ø�
    float4 color = g_texCube.Sample(g_sam_0, input.uvw);

    return color;
}

#endif
