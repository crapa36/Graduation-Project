#pragma once
#include "Component.h"
#include "Mesh.h"

class Material;
class StructuredBuffer;
class Mesh;

class Animator : public Component {
public:
    Animator();
    virtual ~Animator();

public:

    // 본 정보를 설정함.
    void SetBones(const vector<BoneInfo>* bones) { _bones = bones; }

    // 애니메이션 클립을 설정함.
    void SetAnimClip(const vector<AnimClipInfo>* animClips);

    // 데이터를 GPU로 전송함.
    void PushData();

    // 애니메이션 개수를 반환함.
    int32 GetAnimCount() { return static_cast<uint32>(_animClips->size()); }

    // 현재 애니메이션 클립 인덱스를 반환함.
    int32 GetCurrentClipIndex() { return _clipIndex; }

    // 애니메이션을 재생함.
    void Play(uint32 idx);

public:

    // 매 프레임마다 호출되어 상태를 업데이트함.
    virtual void FinalUpdate() override;

private:
    const vector<BoneInfo>* _bones; // 본 정보를 저장하는 벡터 포인터
    const vector<AnimClipInfo>* _animClips; // 애니메이션 클립 정보를 저장하는 벡터 포인터

    float							_updateTime = 0.f; // 애니메이션의 현재 시간을 추적함
    int32							_clipIndex = 0; // 현재 재생 중인 애니메이션 클립의 인덱스
    int32							_frame = 0; // 현재 프레임
    int32							_nextFrame = 0; // 다음 프레임
    float							_frameRatio = 0; // 현재 프레임과 다음 프레임 사이의 비율

    shared_ptr<Material>			_computeMaterial; // GPU 계산에 사용되는 재질
    shared_ptr<StructuredBuffer>	_boneFinalMatrix;  // 특정 프레임의 최종 행렬을 저장하는 구조화된 버퍼
    bool							_boneFinalUpdated = false; // 최종 행렬이 업데이트되었는지 여부를 추적함
};