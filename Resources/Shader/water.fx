#include "params.fx"

struct VS_INPUT
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
    float3 normal : TEXCOORD2;
};

PS_INPUT VS_Main(VS_INPUT input)
{
    PS_INPUT output;

    // �ĵ� ȿ�� ����
    float waveHeight = sin(input.position.x * 0.1 + g_totalTime) * 0.5 +
                       sin(input.position.z * 0.1 + g_totalTime) * 0.5;
    float3 displacedPos = input.position + float3(0.0, waveHeight, 0.0);

    // ���� �������� ��ȯ
    float4 worldPos = mul(float4(displacedPos, 1.0), g_matWorld);
    output.worldPos = worldPos.xyz;

    // �� �������� ��ȯ
    float4 viewPos = mul(worldPos, g_matView);

    // �������� �������� ��ȯ
    output.position = mul(viewPos, g_matProjection);

    // �ؽ�ó ��ǥ ����
    output.texCoord = input.texCoord;

    // ���� ���� (������ Y�� ����)
    output.normal = normalize(mul(float4(input.normal + float3(0.0, waveHeight, 0.0), 0.0), g_matWorld).xyz);

    return output;
} 

float4 PS_Main(PS_INPUT input) : SV_TARGET
{
    // �ü� ���� ��� (ī�޶� ��ġ�� (0,10,0)�̶�� ����)
    float3 viewPos = float3(0.0, 10.0, 0.0);
    float3 viewDir = normalize(viewPos - input.worldPos);

    // �ݻ� ���� ���
    float3 normal = normalize(input.normal);
    float3 reflectedDir = reflect(viewDir, normal);
    float4 reflectionColor = g_texCube.Sample(g_sam_0, reflectedDir);

    // ���� ���� ���
    float3 refractedDir = refract(viewDir, normal, 0.9);
    float4 refractionColor = g_refractionTex.Sample(g_sam_0, refractedDir.xy * 0.5 + 0.5); // 2D �ؽ�ó�� ����

    // ������ ȿ�� ���
    float fresnel = pow(1.0 - saturate(dot(viewDir, normal)), 3.0);

    // �ݻ�� ���� ���� ȥ��
    float4 finalColor = lerp(refractionColor, reflectionColor, fresnel);

    // �� �ؽ�ó �ְ�
    float2 distortedTex = input.texCoord + float2(sin(input.worldPos.x * 0.1 + g_totalTime), cos(input.worldPos.z * 0.1 + g_totalTime)) * 0.02;
    float4 waterTexColor = float4(0.f, 0.f, 1.0f, 1.0f);

    finalColor *= waterTexColor;

    return finalColor;
}