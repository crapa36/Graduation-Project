#ifndef _DEFAULT_FX_
#define _DEFAULT_FX_

#include "params.fx"
#include "utils.fx"

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 weight : WEIGHT;
    float4 indices : INDICES;

    row_major matrix matWorld : W;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;
    uint instanceID : SV_InstanceID;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float3 viewBinormal : BINORMAL;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    if (g_int_0 == 1)
    {
        if (g_int_1 == 1)
            Skinning(input.pos, input.normal, input.tangent, input.weight, input.indices);

        output.pos = mul(float4(input.pos, 1.f), input.matWVP);
        output.uv = input.uv;

        output.viewPos = mul(float4(input.pos, 1.f), input.matWV).xyz;
        output.viewNormal = normalize(mul(float4(input.normal, 0.f), input.matWV).xyz);
        output.viewTangent = normalize(mul(float4(input.tangent, 0.f), input.matWV).xyz);
        output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));
    }
    else
    {
        if (g_int_1 == 1)
            Skinning(input.pos, input.normal, input.tangent, input.weight, input.indices);

        output.pos = mul(float4(input.pos, 1.f), g_matWVP);
        output.uv = input.uv;

        output.viewPos = mul(float4(input.pos, 1.f), g_matWV).xyz;
        output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);
        output.viewTangent = normalize(mul(float4(input.tangent, 0.f), g_matWV).xyz);
        output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));
    }

    return output;
}
struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2; // Albedo
    float4 material : SV_Target3; // Metallic, Roughness, Ambient Occlusion
};

PS_OUT PS_Main(VS_OUT input)
{
    PS_OUT output = (PS_OUT) 0;

    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    if (g_tex_ons[0] == 1)
        color = g_textures[0].Sample(g_sam_0, input.uv);

    float3 viewNormal = input.viewNormal;
    if (g_tex_ons[1] == 1)
    {
        float3 tangentSpaceNormal = g_textures[1].Sample(g_sam_0, input.uv).xyz;
        tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.f;
        float3x3 matTBN = { input.viewTangent, input.viewBinormal, input.viewNormal };
        viewNormal = normalize(mul(tangentSpaceNormal, matTBN));
    }

    // 추가된 속성
    float metallic = g_tex_ons[2] == 1 ? g_textures[2].Sample(g_sam_0, input.uv).r : 0.f;
    float roughness = g_tex_ons[3] == 1 ? g_textures[3].Sample(g_sam_0, input.uv).r : 0.5f;
    float ao = g_tex_ons[4] == 1 ? g_textures[4].Sample(g_sam_0, input.uv).r : 1.f;

    output.position = float4(input.viewPos.xyz, 0.f);
    output.normal = float4(viewNormal.xyz, 0.f);
    output.color = color;
    output.material = float4(metallic, roughness, ao, 0.f);

    return output;
}


#endif