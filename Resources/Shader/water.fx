#ifndef _WATER_FX_
#define _WATER_FX_

#include "params.fx"

// ���� ���̴��� ���޵� �Է� ����ü
struct VS_IN
{
    float3 pos : POSITION; // ���� ��ġ
    float2 uv : TEXCOORD; // �ؽ�ó ��ǥ
    float3 normal : NORMAL; // ���� ���
    float3 tangent : TANGENT; // ���� ���� (Tangent)
};

// ���� ���̴����� �ȼ� ���̴��� ���޵Ǵ� ��� ����ü
struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float3 viewBinormal : BINORMAL;
};

// ���� ���̴�: ������ ��ȯ�ϰ� �ʿ��� �����͸� �ȼ� ���̴��� ����
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

// �ȼ� ���̴�: ���� �⺻ ����(�Ķ���)�� �����ϰ� �ݻ�, ����, ��ָ� ȿ���� �߰�
float4 PS_Main(VS_OUT input) : SV_TARGET
{
    // �⺻ �� ���� - �Ķ���
    float4 baseColor = float4(0.0, 0.5, 1.0, 1.0); // �Ķ��� �⺻ ����

    // �⺻ ����� ���
    float3 perturbedNormal = normalize(input.viewNormal);

    // �ݻ� ���� ���
    float3 viewDir = normalize(g_matViewInv[3].xyz - input.viewPos);
    float3 reflectionVector = reflect(viewDir, perturbedNormal);

    // ���� ���� ��� (��ֿ� ����Ͽ� ����)
    float3 refractionVector = refract(viewDir, perturbedNormal, 0.97); // ���� ������(�� 0.97)

    // �ݻ� �ؽ�ó ���ø� (ť�� ���� �̿��� ȯ�� ����)
    float4 reflectionColor = g_texCube.Sample(g_sam_0, reflectionVector);

    // ���� �ؽ�ó ���ø� (�� ǥ���� �ְ��� ǥ��)
    float4 refractionColor = g_refractionTex.Sample(g_sam_0, input.uv); // ��ָ� ������� ����

    // �ݻ�� ���� ���� ȥ��
    float4 finalColor = lerp(refractionColor, reflectionColor, 0.5);

    // �⺻ �� ���� �ݻ�� ������ ȥ���Ͽ� ���� ���� ���
    finalColor = lerp(baseColor, reflectionColor, 0.5);

    finalColor.a = 0.5;

    return finalColor;
}

#endif