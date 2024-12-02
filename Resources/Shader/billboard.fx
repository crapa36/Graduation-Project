#include "params.fx"
#include "utils.fx"

struct Billboard
{
    float3 worldPos;
    float3 worldDir;
};

StructuredBuffer<Billboard> g_data : register(t9);

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VS_OUT
{
    float4 viewPos : POSITION;
    float2 uv : TEXCOORD;
    float3 worldPos : TEXCOORD1; // 카메라 고정을 위해 월드 위치 추가
};

// VS_MAIN
// g_tex_0      : Particle Texture

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0.f;

    float3 worldPos = mul(float4(input.pos, 1.f), g_matWorld).xyz;

    output.viewPos = mul(float4(worldPos, 1.f), g_matView);
    output.uv = input.uv;
    output.worldPos = worldPos; // 월드 위치 전달
    return output;
}

struct GS_OUT
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
};
// GS_Main
// g_vec4_0     : Rotation Axis
// g_vec4_1     : Scale
[maxvertexcount(6)]
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUT> outputStream)
{
    GS_OUT output[4] =
    {
        (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f
    };

    VS_OUT vtx = input[0];

    float scaleX = g_vec4_1.x; // X축 스케일
    float scaleY = g_vec4_1.y; // Y축 스케일



    // 카메라의 위치 (View Space에서 원래 월드 위치로 변환 필요)
    float3 cameraPosWS = mul(float4(0, 0, 0, 1), g_matViewInv).xyz; // g_matInvView는 View 행렬의 역행렬
     // 회전 기준 축 (정규화)
    float3 rotationAxis = normalize(g_vec4_0.xyz);
    // 유효한 축인지 확인
    if (length(rotationAxis) < 0.001f)
    {
        rotationAxis = float3(0, 1, 0); // 기본 Y축
    }

    // 빌보드 방향 계산 (회전 축을 기준으로)
    float3 lookDir = normalize(vtx.worldPos - cameraPosWS);
    // 회전 축을 기준으로 lookDir을 평행 투영
    lookDir = normalize(lookDir - dot(lookDir, rotationAxis) * rotationAxis);
    
    // 오른쪽과 위쪽 벡터 계산
    float3 right = normalize(cross(float3(0, 1, 0), lookDir));
    float3 up = float3(0, 1, 0);

    // 빌보드의 4개 코너 계산
    float3 corner1 = vtx.worldPos + (-right * scaleX + up * scaleY);
    float3 corner2 = vtx.worldPos + (right * scaleX + up * scaleY);
    float3 corner3 = vtx.worldPos + (right * scaleX - up * scaleY);
    float3 corner4 = vtx.worldPos + (-right * scaleX - up * scaleY);

    // View Space로 변환
    float4 viewPos1 = mul(float4(corner1, 1.f), g_matView);
    float4 viewPos2 = mul(float4(corner2, 1.f), g_matView);
    float4 viewPos3 = mul(float4(corner3, 1.f), g_matView);
    float4 viewPos4 = mul(float4(corner4, 1.f), g_matView);

    // Projection Space로 변환
    viewPos1 = mul(viewPos1, g_matProjection);
    viewPos2 = mul(viewPos2, g_matProjection);
    viewPos3 = mul(viewPos3, g_matProjection);
    viewPos4 = mul(viewPos4, g_matProjection);

    // UV 설정
    output[0].uv = float2(0.f, 0.f);
    output[1].uv = float2(1.f, 0.f);
    output[2].uv = float2(1.f, 1.f);
    output[3].uv = float2(0.f, 1.f);

    // Position 설정
    output[0].position = viewPos1;
    output[1].position = viewPos2;
    output[2].position = viewPos3;
    output[3].position = viewPos4;

    // 두 개의 삼각형으로 빌보드 그리기
    outputStream.Append(output[0]);
    outputStream.Append(output[1]);
    outputStream.Append(output[2]);
    outputStream.RestartStrip();

    outputStream.Append(output[0]);
    outputStream.Append(output[2]);
    outputStream.Append(output[3]);
    outputStream.RestartStrip();
}

float4 PS_Main(GS_OUT input) : SV_Target
{
    return g_textures[0].Sample(g_sam_0, input.uv);
}
