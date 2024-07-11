#include "pch.h"
#include "Animator.h"
#include "Timer.h"
#include "Resources.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "StructuredBuffer.h"

Animator::Animator() : Component(COMPONENT_TYPE::ANIMATOR) {

    // ComputeAnimation 재질을 로드하여 _computeMaterial에 할당함
    _computeMaterial = GET_SINGLETON(Resources)->Get<Material>(L"ComputeAnimation");

    // 본 최종 행렬을 위한 구조화된 버퍼를 초기화함
    _boneFinalMatrix = make_shared<StructuredBuffer>();
}

Animator::~Animator() {
}

void Animator::FinalUpdate() {
    _updateTime += DELTA_TIME; // DELTA_TIME을 사용하여 _updateTime을 업데이트함

    const AnimClipInfo& animClip = _animClips->at(_clipIndex); // 현재 애니메이션 클립 정보를 가져옴
    if (_updateTime >= animClip.duration)
        _updateTime = 0.f; // 애니메이션 클립의 총 재생 시간을 초과하면 _updateTime을 0으로 리셋함

    const int32 ratio = static_cast<int32>(animClip.frameCount / animClip.duration); // 프레임 비율을 계산함
    _frame = static_cast<int32>(_updateTime * ratio); // 현재 프레임을 계산함
    _frame = min(_frame, animClip.frameCount - 1); // 계산된 현재 프레임이 애니메이션의 총 프레임 수를 초과하지 않도록 함
    _nextFrame = min(_frame + 1, animClip.frameCount - 1); // 다음 프레임을 계산함
    _frameRatio = static_cast<float>(_frame - _frame); // 현재 프레임과 다음 프레임 사이의 비율을 계산함 (현재 사용되지 않음)
}

void Animator::SetAnimClip(const vector<AnimClipInfo>* animClips) {
    _animClips = animClips; // 애니메이션 클립 정보를 설정함
}

void Animator::PushData() {
    uint32 boneCount = static_cast<uint32>(_bones->size()); // 본 개수를 계산함
    if (_boneFinalMatrix->GetElementCount() < boneCount)
        _boneFinalMatrix->Init(sizeof(Matrix), boneCount); // 필요한 경우 본 최종 행렬 버퍼의 크기를 조정함

    // Compute Shader를 사용하여 본 애니메이션을 계산함
    shared_ptr<Mesh> mesh = GetGameObject()->GetMeshRenderer()->GetMesh();
    mesh->GetBoneFrameDataBuffer(_clipIndex)->PushComputeSRVData(SRV_REGISTER::t8);
    mesh->GetBoneOffsetBuffer()->PushComputeSRVData(SRV_REGISTER::t9);

    _boneFinalMatrix->PushComputeUAVData(UAV_REGISTER::u0);

    _computeMaterial->SetInt(0, boneCount);
    _computeMaterial->SetInt(1, _frame);
    _computeMaterial->SetInt(2, _nextFrame);
    _computeMaterial->SetFloat(0, _frameRatio);

    // 본 처리를 위해 필요한 최소 스레드 그룹 수를 계산. 256개 스레드로 나누고, 나머지 처리를 위해 +1
    uint32 groupCount = (boneCount / 256) + 1;
    _computeMaterial->Dispatch(groupCount, 1, 1); // Compute Shader를 실행함

    // Graphics Shader에 최종 본 행렬 데이터를 전송함
    _boneFinalMatrix->PushGraphicsData(SRV_REGISTER::t7);
}

void Animator::Play(uint32 idx) {
    assert(idx < _animClips->size()); // 재생할 애니메이션 클립의 인덱스가 유효한지 확인함
    _clipIndex = idx; // 애니메이션 클립 인덱스를 설정함
    _updateTime = 0.f; // _updateTime을 리셋함
}