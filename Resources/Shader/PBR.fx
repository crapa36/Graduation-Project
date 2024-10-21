#include "params.fx"

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float3 normal : TEXCOORD0;
    float2 texCoord : TEXCOORD1;
};

VS_OUT VS_Main(float3 pos : POSITION, float3 normal : NORMAL, float2 texCoord : TEXCOORD0)
{
    return VS(pos, normal, texCoord);
}

float4 PS_Main(VS_OUT input) : SV_TARGET
{
    // G-Buffer 패스를 통해 각종 정보를 저장
    PS_OUT gbuffer = PS(input);

    // Lighting 패스에서 G-Buffer로부터 데이터 읽기
    PS_IN ps_input;
    ps_input.pos = input.pos;
    ps_input.texCoord = input.texCoord;

    // 조명 계산
    return PS(ps_input);
}