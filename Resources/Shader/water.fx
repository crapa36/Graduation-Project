// WaterPBR.hlsl

#include "params.fx"

// 정점 입력 구조체
struct VS_INPUT
{
    float3 pos : POSITION; // 정점 위치
    float3 normal : NORMAL; // 정점 법선
    float2 uv : TEXCOORD0; // 텍스처 좌표
};

// 픽셀 셰이더 입력 구조체
struct PS_INPUT
{
    float4 pos : SV_POSITION; // 스크린 공간 위치
    float3 worldPos : TEXCOORD0; // 월드 공간 위치
    float3 normal : TEXCOORD1; // 월드 공간 법선
    float2 uv : TEXCOORD2; // 텍스처 좌표
    float3 viewDir : TEXCOORD3; // 뷰 방향
};

// 정점 셰이더
PS_INPUT VS_Main(VS_INPUT input)
{
    PS_INPUT output;

    // 월드 공간 위치 계산
    float4 worldPos = mul(float4(input.pos, 1.0), g_matWorld);
    output.worldPos = worldPos.xyz;

    // 뷰 공간 위치 계산
    float4 viewPos = mul(worldPos, g_matView);
    output.pos = mul(viewPos, g_matProjection);

    // 월드 공간 법선 계산 및 정규화
    output.normal = normalize(mul(input.normal, (float3x3) g_matWorld));
    
    // 뷰 방향 계산
    output.viewDir = normalize(g_matViewInv[3].xyz - worldPos.xyz);

    // 텍스처 좌표 전달
    output.uv = input.uv;

    return output;
}

// 픽셀 셰이더
float4 PS_Main(PS_INPUT input) : SV_TARGET
{
    // 보간된 법선 정규화
    float3 N = normalize(input.normal);
    // 뷰 방향 정규화
    float3 V = normalize(input.viewDir);
    // 반사 벡터 계산 (간단화를 위해 가정)
    float3 R = reflect(-V, N);
    
    // 반사 텍스처와 굴절 텍스처 샘플링
    float4 reflectionColor = g_texCube.Sample(g_sam_0, R);
    float4 refractionColor = g_refractionTex.Sample(g_sam_0, input.uv);

    // 프레넬 계수 계산
    float fresnel = pow(1.0 - saturate(dot(N, V)), 0.5f);

    // 프레넬 계수에 기반하여 반사와 굴절 색상 혼합
    float4 color = refractionColor;
    //float4 color = lerp(refractionColor, reflectionColor, fresnel * 0.5f);

    // 물 색상 적용
    color *= float4(0.0f, 0.0f, 0.5f, 1.0f);

    // 간단한 조명 계산 (램버트 조명 모델)
    float3 lighting = float3(0, 0, 0);
    for (int i = 0; i < g_lightCount; ++i)
    {
        LightInfo light = g_light[i];
        float3 L;
        float attenuation = 1.0;

        if (light.lightType == 0) // 방향성 광원
        {
            L = normalize(light.direction.xyz);
            attenuation = 1.0;
        }
        else // 점 광원
        {
            L = normalize(light.position.xyz - input.worldPos);
            float distance = length(light.position.xyz - input.worldPos);
            attenuation = saturate(1.0 - (distance / light.range));
        }

        // 확산 조명 계산
        float diff = max(dot(N, L), 0.0);
        lighting += diff * light.color.diffuse.rgb * attenuation;

        // 반사 조명 계산 (블린-펭 모델)
        float3 H = normalize(L + V);
        float spec = pow(max(dot(N, H), 0.0), 16.0);
        lighting += spec * light.color.specular.rgb * attenuation;
    }

    // 최종 색상에 조명 적용 (첫 번째 조명의 주변 조명 포함)
    color.rgb *= lighting + g_light[0].color.ambient.rgb;

    return color;
}