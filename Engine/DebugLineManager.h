// DebugLineManager.h

#pragma once

#include "pch.h"
#include "Shader.h"
#include "Math.h"

struct LineVertex {
    Vec3 position;
    Vec4 color;
};

class DebugLineManager{
public:
    DECLARE_SINGLETON(DebugLineManager);

    // 디버그 라인을 추가합니다.
    void AddLine(const Vec3& start, const Vec3& end, const Vec4& color= {0.f, 0.f, 0.f, 1.0});

    // 저장된 디버그 라인들을 모두 삭제합니다.
    void Clear();

    // 디버그 라인을 렌더링합니다.
    void Render();

private:
    // 라인 데이터를 위한 Shader Resource View(SRV)를 생성합니다.
    D3D12_CPU_DESCRIPTOR_HANDLE CreateLineDataSRV();

private:
    // 디버그 라인의 정점 데이터를 저장하는 벡터입니다.
    std::vector<LineVertex> _lineVertices;



    // 정점 버퍼 리소스입니다.
    ComPtr<ID3D12Resource> _vertexBuffer;

    // 디버그 라인 렌더링에 사용될 셰이더입니다.
    std::shared_ptr<Shader> _shader;
};
