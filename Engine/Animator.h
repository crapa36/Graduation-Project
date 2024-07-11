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

    // �� ������ ������.
    void SetBones(const vector<BoneInfo>* bones) { _bones = bones; }

    // �ִϸ��̼� Ŭ���� ������.
    void SetAnimClip(const vector<AnimClipInfo>* animClips);

    // �����͸� GPU�� ������.
    void PushData();

    // �ִϸ��̼� ������ ��ȯ��.
    int32 GetAnimCount() { return static_cast<uint32>(_animClips->size()); }

    // ���� �ִϸ��̼� Ŭ�� �ε����� ��ȯ��.
    int32 GetCurrentClipIndex() { return _clipIndex; }

    // �ִϸ��̼��� �����.
    void Play(uint32 idx);

public:

    // �� �����Ӹ��� ȣ��Ǿ� ���¸� ������Ʈ��.
    virtual void FinalUpdate() override;

private:
    const vector<BoneInfo>* _bones; // �� ������ �����ϴ� ���� ������
    const vector<AnimClipInfo>* _animClips; // �ִϸ��̼� Ŭ�� ������ �����ϴ� ���� ������

    float							_updateTime = 0.f; // �ִϸ��̼��� ���� �ð��� ������
    int32							_clipIndex = 0; // ���� ��� ���� �ִϸ��̼� Ŭ���� �ε���
    int32							_frame = 0; // ���� ������
    int32							_nextFrame = 0; // ���� ������
    float							_frameRatio = 0; // ���� �����Ӱ� ���� ������ ������ ����

    shared_ptr<Material>			_computeMaterial; // GPU ��꿡 ���Ǵ� ����
    shared_ptr<StructuredBuffer>	_boneFinalMatrix;  // Ư�� �������� ���� ����� �����ϴ� ����ȭ�� ����
    bool							_boneFinalUpdated = false; // ���� ����� ������Ʈ�Ǿ����� ���θ� ������
};