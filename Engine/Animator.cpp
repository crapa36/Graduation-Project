#include "pch.h"
#include "Animator.h"
#include "Timer.h"
#include "Resources.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "StructuredBuffer.h"

Animator::Animator() : Component(COMPONENT_TYPE::ANIMATOR) {

    // ComputeAnimation ������ �ε��Ͽ� _computeMaterial�� �Ҵ���
    _computeMaterial = GET_SINGLETON(Resources)->Get<Material>(L"ComputeAnimation");

    // �� ���� ����� ���� ����ȭ�� ���۸� �ʱ�ȭ��
    _boneFinalMatrix = make_shared<StructuredBuffer>();
}

Animator::~Animator() {
}

void Animator::FinalUpdate() {
    _updateTime += DELTA_TIME; // DELTA_TIME�� ����Ͽ� _updateTime�� ������Ʈ��

    const AnimClipInfo& animClip = _animClips->at(_clipIndex); // ���� �ִϸ��̼� Ŭ�� ������ ������
    if (_updateTime >= animClip.duration)
        _updateTime = 0.f; // �ִϸ��̼� Ŭ���� �� ��� �ð��� �ʰ��ϸ� _updateTime�� 0���� ������

    const int32 ratio = static_cast<int32>(animClip.frameCount / animClip.duration); // ������ ������ �����
    _frame = static_cast<int32>(_updateTime * ratio); // ���� �������� �����
    _frame = min(_frame, animClip.frameCount - 1); // ���� ���� �������� �ִϸ��̼��� �� ������ ���� �ʰ����� �ʵ��� ��
    _nextFrame = min(_frame + 1, animClip.frameCount - 1); // ���� �������� �����
    _frameRatio = static_cast<float>(_frame - _frame); // ���� �����Ӱ� ���� ������ ������ ������ ����� (���� ������ ����)
}

void Animator::SetAnimClip(const vector<AnimClipInfo>* animClips) {
    _animClips = animClips; // �ִϸ��̼� Ŭ�� ������ ������
}

void Animator::PushData() {
    uint32 boneCount = static_cast<uint32>(_bones->size()); // �� ������ �����
    if (_boneFinalMatrix->GetElementCount() < boneCount)
        _boneFinalMatrix->Init(sizeof(Matrix), boneCount); // �ʿ��� ��� �� ���� ��� ������ ũ�⸦ ������

    // Compute Shader�� ����Ͽ� �� �ִϸ��̼��� �����
    shared_ptr<Mesh> mesh = GetGameObject()->GetMeshRenderer()->GetMesh();
    mesh->GetBoneFrameDataBuffer(_clipIndex)->PushComputeSRVData(SRV_REGISTER::t8);
    mesh->GetBoneOffsetBuffer()->PushComputeSRVData(SRV_REGISTER::t9);

    _boneFinalMatrix->PushComputeUAVData(UAV_REGISTER::u0);

    _computeMaterial->SetInt(0, boneCount);
    _computeMaterial->SetInt(1, _frame);
    _computeMaterial->SetInt(2, _nextFrame);
    _computeMaterial->SetFloat(0, _frameRatio);

    // �� ó���� ���� �ʿ��� �ּ� ������ �׷� ���� ���. 256�� ������� ������, ������ ó���� ���� +1
    uint32 groupCount = (boneCount / 256) + 1;
    _computeMaterial->Dispatch(groupCount, 1, 1); // Compute Shader�� ������

    // Graphics Shader�� ���� �� ��� �����͸� ������
    _boneFinalMatrix->PushGraphicsData(SRV_REGISTER::t7);
}

void Animator::Play(uint32 idx) {
    assert(idx < _animClips->size()); // ����� �ִϸ��̼� Ŭ���� �ε����� ��ȿ���� Ȯ����
    _clipIndex = idx; // �ִϸ��̼� Ŭ�� �ε����� ������
    _updateTime = 0.f; // _updateTime�� ������
}