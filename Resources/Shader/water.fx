#ifndef _WATER_FX_
#define _WATER_FX_

#include "params.fx"

// ���� ���̴��� ���޵� �Է� ����ü
struct VS_INPUT
{
    float3 pos : POSITION; // ���� ��ġ
    float3 normal : NORMAL; // ���� ���
    float2 uv : TEXCOORD0; // �ؽ�ó ��ǥ
    float4 tangent : TANGENT; // ���� ���� (Tangent)
};

// ���� ���̴����� �ȼ� ���̴��� ���޵Ǵ� ��� ����ü
struct VS_OUTPUT
{
    float4 pos : SV_POSITION; // Ŭ�� ���������� ��ġ
    float2 uv : TEXCOORD0; // �ؽ�ó ��ǥ
    float3 normal : NORMAL; // ���� ���������� ���
    float3 tangent : TANGENT; // ���� ���������� ���� ����
    float3 binormal : BINORMAL; // ���� ���������� ���̳�� ����
    float3 worldPos : TEXCOORD1; // ���� ���������� ��ġ
};

// ���� ���̴�: ������ ��ȯ�ϰ� �ʿ��� �����͸� �ȼ� ���̴��� ����
VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output;

    // ���� �������� ���� ��ġ ��ȯ
    float4 worldPos = mul(float4(input.pos, 1.0f), g_matWorld);
    output.worldPos = worldPos.xyz;

    // ������ ���̳�� ���
    float3 worldNormal = mul(input.normal, (float3x3) g_matWorld);
    float3 worldTangent = mul(input.tangent.xyz, (float3x3) g_matWorld);
    float3 worldBinormal = cross(worldNormal, worldTangent) * input.tangent.w;

    output.normal = worldNormal;
    output.tangent = worldTangent;
    output.binormal = worldBinormal;

    // ���� ��ġ�� Ŭ�� �������� ��ȯ
    output.pos = mul(worldPos, g_matWVP);

    // �ؽ�ó ��ǥ ����
    output.uv = input.uv;

    return output;
}

// �ȼ� ���̴�: ���� �⺻ ����(�Ķ���)�� �����ϰ� �ݻ�, ����, ��ָ� ȿ���� �߰�
float4 PS_Main(VS_OUTPUT input) : SV_TARGET
{
    // �⺻ �� ���� - �Ķ���
    float4 baseColor = float4(0.0, 0.5, 1.0, 1.0); // �Ķ��� �⺻ ����

    // ��ָ��� ���ø� (����� ����)
    float3 normalMap = g_tex_0.Sample(g_sam_0, input.uv).rgb;
    normalMap = normalize(normalMap * 2.0f - 1.0f); // [0,1] �������� [-1,1]�� ��ȯ

    // ����� ���� �������� ��ȯ
    float3 perturbedNormal = normalize(normalMap.x * input.tangent + normalMap.y * input.binormal + normalMap.z * input.normal);

    // �ݻ� ���� ���
    float3 viewDir = normalize(g_matViewInv[3].xyz - input.worldPos);
    float3 reflectionVector = reflect(viewDir, perturbedNormal);

    // ���� ���� ��� (��ֿ� ����Ͽ� ����)
    float3 refractionVector = refract(viewDir, perturbedNormal, 0.97); // ���� ������(�� 0.97)

    // �ݻ� �ؽ�ó ���ø� (ť�� ���� �̿��� ȯ�� ����)
    float4 reflectionColor = g_texCube.Sample(g_sam_0, reflectionVector);

    // ���� �ؽ�ó ���ø� (�� ǥ���� �ְ��� ǥ��)
    float4 refractionColor = g_refractionTex.Sample(g_sam_0, input.uv + normalMap.xy * 0.1);

    // �ݻ�� ���� ���� ȥ��
    float4 finalColor = lerp(refractionColor, reflectionColor, 0.5);

    // �⺻ �� ���� �ݻ�� ������ ȥ���Ͽ� ���� ���� ���
    finalColor = lerp(baseColor, finalColor, 0.5);

    return finalColor;
}

#endif