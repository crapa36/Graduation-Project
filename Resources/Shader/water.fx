// water_vs.hlsl
#include "params.fx"

struct VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
    float3 tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float3 worldPos : TEXCOORD;
    float3 normal : TEXCOORD1;
    float2 texCoord : TEXCOORD2;
    float3 tangent : TEXCOORD3;
    float3 viewDir : TEXCOORD4;
};

VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output;

    // ���� �������� ��ȯ
    float4 worldPos = mul(float4(input.pos, 1.0f), g_matWorld);
    output.worldPos = worldPos.xyz;

    // �� ���� ���
    float3 viewPosition = mul(float4(0, 0, 0, 1), g_matViewInv).xyz; // ī�޶� ��ġ
    output.viewDir = normalize(viewPosition - output.worldPos);

    // ���� ��ġ ��ȯ
    output.pos = mul(worldPos, g_matView);
    output.pos = mul(output.pos, g_matProjection);

    // ��ְ� ź��Ʈ ��ȯ
    output.normal = mul(float4(input.normal, 0.0f), g_matWorld).xyz;
    output.tangent = mul(float4(input.tangent, 0.0f), g_matWorld).xyz;

    // �ؽ�ó ��ǥ ����
    output.texCoord = input.texCoord;

    return output;
}


// PBR ����ü
struct PBRMaterial
{
    float3 albedo;
    float metallic;
    float roughness;
    float3 normalMap;
};

// �Լ�: ��� ���� ��ȯ (TBN ��Ʈ���� ���)
float3 TransformNormal(float3 normal, float3 tangent, float3 bitangent)
{
    float3x3 TBN = float3x3(normalize(tangent), normalize(bitangent), normalize(normal));
    return normalize(mul(normal, TBN));
}

// Fresnel �Լ�
float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

// ���� Pixel Shader
float4 PS_Main(VS_OUTPUT input) : SV_Target
{
    // ��� �� ���ø�
    float3 normal = normalize(input.normal);
    // ��� ���� �����Ϸ��� �߰� �ڵ� �ʿ� (��: tangent space���� world space�� ��ȯ)

    // �⺻ PBR �Ķ���� (���� ��)
    PBRMaterial material;
    material.albedo = float3(0.0f, 0.3f, 0.5f); // ���� �⺻ ����
    material.metallic = 0.0f;
    material.roughness = 0.1f;
    material.normalMap = normal;

    // �⺻ ���� ���
    float3 N = normalize(normal);
    float3 V = normalize(input.viewDir);

    // ȯ�� ���� ����� �ݻ�
    float3 R = reflect(-V, N);
    float3 reflection = g_texCube.Sample(g_sam_0, R).rgb;

    // ���� ���
    float3 refraction = g_refractionTex.Sample(g_sam_0, input.texCoord).rgb;

    // Fresnel ȿ��
    float3 F0 = float3(0.04f, 0.04f, 0.04f); // ��ݼ� ������ �⺻ F0
    float3 F = FresnelSchlick(max(dot(N, V), 0.0f), F0);

    // ���� ���� ���
    float3 color = lerp(refraction, reflection, F);

    // ������ ������ (�߰����� PBR ������ ���� ������ �� ����)
    for (int i = 0; i < g_lightCount; ++i)
    {
        LightInfo light = g_light[i];
        float3 L = normalize(light.position.xyz - input.worldPos);
        float3 H = normalize(L + V);
        float NdotL = max(dot(N, L), 0.0f);
        float NdotH = max(dot(N, H), 0.0f);
        float NdotV = max(dot(N, V), 0.0f);
        float VdotH = max(dot(V, H), 0.0f);

        // �ſ� �ݻ� ��
        float alpha = pow(1.0f - material.roughness, 2.0f);
        float D = (alpha * alpha) / (3.14159265359f * pow((NdotH * NdotH) * (alpha * alpha - 1.0f) + 1.0f, 2.0f));

        // �������� ���� �� (Schlick-GGX)
        float k = (alpha + 1.0f) * (alpha + 1.0f) / 8.0f;
        float G = min(1.0f, min((2.0f * NdotH * NdotV) / VdotH, (2.0f * NdotH * NdotL) / VdotH));

        // Fresnel �� (�̹� ����)
        // float3 F = FresnelSchlick(max(dot(N, V), 0.0f), F0);

        // ���� BRDF
        float3 specular = (D * F * G) / (4.0f * NdotV * NdotL + 0.001f);
        float3 diffuse = material.albedo / 3.14159265359f;

        color += (diffuse * light.color.ambient + (diffuse + specular) * light.color.diffuse) * NdotL;
    }

    // ���� ���� Ŭ����
    color = saturate(color);

    return float4(color, 1.0f);
}