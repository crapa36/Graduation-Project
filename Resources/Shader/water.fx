#ifndef _WATER_FX_
#define _WATER_FX_

#include "params.fx"  // params.fx ������ ����

// ���ؽ� ���̴� �Է� ����ü
struct VS_IN
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
};

// ���ؽ� ���̴� ��� ����ü
struct VS_OUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
    float3 Normal : TEXCOORD2;
};

// ���ؽ� ���̴�
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;

    // ���� ��ǥ������ ��ġ�� ��� ���
    float4 worldPos = mul(float4(input.Pos, 1.0f), g_matWorld);
    output.WorldPos = worldPos.xyz;

    float4 viewPos = mul(worldPos, g_matView);
    output.Pos = mul(viewPos, g_matProjection);

    output.Normal = normalize(mul(float4(input.Normal, 0.0f), g_matWorld).xyz);
    output.TexCoord = input.TexCoord;

    return output;
}

// �ȼ� ���̴� �Է� ����ü
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
    float3 Normal : TEXCOORD2;
};

// �� �ȼ� ���̴�
float4 PS_Main(PS_INPUT input) : SV_TARGET
{
    // �� ����� �ݻ� ���� ���
    float3 viewDir = normalize(input.WorldPos - g_matViewInv[3].xyz);
    float3 reflectDir = reflect(viewDir, input.Normal);

    // �ݻ� �ؽ�ó ���ø�
    float4 reflection = g_reflectionTex.Sample(g_sam_0, reflectDir.xy * 0.5 + 0.5);
    // ���� �ؽ�ó ���ø�
    float4 refraction = g_refractionTex.Sample(g_sam_0, input.TexCoord);

    // ���� ������ �ݻ�, ������ ����
    float4 waterColor = lerp(refraction, reflection, 0.4); // �ݻ�� ������ ȥ��
    waterColor.a = 0.5; // ���� �� ���� (����)

    return waterColor;
}
#endif