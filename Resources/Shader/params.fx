#ifndef _PARAMS_FX_
#define _PARAMS_FX_

//�ؽ��� �迭 ũ�� ����
#define MAX_TEXTURES 8

struct LightColor
{
    float4 diffuse;
    float4 ambient;
    float4 specular;
};

struct LightInfo
{
    LightColor color;
    float4 position;
    float4 direction;
    int lightType;
    float range;
    float angle;
    int padding;
};

cbuffer GLOBAL_PARAMS : register(b0)
{
    int g_lightCount;
    float3 g_lightPadding;
    LightInfo g_light[50];
}

cbuffer TRANSFORM_PARAMS : register(b1)
{
    row_major matrix g_matWorld;
    row_major matrix g_matView;
    row_major matrix g_matProjection;
    row_major matrix g_matWV;
    row_major matrix g_matWVP;
    row_major matrix g_matViewInv;
    row_major matrix g_matProjectionInv;
};

cbuffer MATERIAL_PARAMS : register(b2)
{
    int g_int_0;
    int g_int_1;
    int g_int_2;
    int g_int_3;
    float g_float_0;
    float g_float_1;
    float g_float_2;
    float g_float_3;
    float2 g_vec2_0;
    float2 g_vec2_1;
    float2 g_vec2_2;
    float2 g_vec2_3;
    float4 g_vec4_0;
    float4 g_vec4_1;
    float4 g_vec4_2;
    float4 g_vec4_3;
    row_major float4x4 g_mat_0;
    row_major float4x4 g_mat_1;
    row_major float4x4 g_mat_2;
    row_major float4x4 g_mat_3;
    int g_tex_ons[MAX_TEXTURES];
};

// Texture2D �ؽ�ó ����
Texture2D g_textures[MAX_TEXTURES] : register(t0);

Texture2D g_depth : register(t1);

// TextureCube �ؽ�ó ���� (ť��� �߰�)
TextureCube g_texCube : register(t6);

Texture2D g_refractionTex : register(t9); // ���� �ؽ�ó

StructuredBuffer<Matrix> g_mat_bone : register(t7);

SamplerState g_sam_0 : register(s0);
#endif