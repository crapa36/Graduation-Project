#pragma once
#include "Component.h"

// ����Ʈ�� Ÿ���� �����ϴ� �������Դϴ�.
enum class LIGHT_TYPE : uint8 {
    DIRECTIONAL, // ���⼺ ����Ʈ
    POINT,       // ����Ʈ ����Ʈ
    SPOT,        // ���� ����Ʈ
};

// ����Ʈ�� ���� ������ ��� ����ü�Դϴ�.
struct LightColor {
    Vec4	diffuse;  // Ȯ�� ����
    Vec4	ambient;  // �ֺ� ����
    Vec4	specular; // �ݻ� ����
};

// ����Ʈ�� ������ ��� ����ü�Դϴ�.
struct LightInfo {
    LightColor	color;      // ����Ʈ�� ���� ����
    Vec4		position;   // ����Ʈ�� ��ġ
    Vec4		direction;  // ����Ʈ�� ����
    int32		lightType;  // ����Ʈ�� Ÿ��
    float		range;      // ����Ʈ�� ����
    float		angle;      // ����Ʈ�� ����
    int32		padding;    // �е�
};

// ����Ʈ �Ķ���͸� ��� ����ü�Դϴ�.
struct LightParams {
    uint32		lightCount; // ����Ʈ�� ����
    Vec3		padding;    // �е�
    LightInfo	lights[50]; // ����Ʈ ���� �迭
};

// ����Ʈ ������Ʈ Ŭ�����Դϴ�.
class Light : public Component {
public:
    Light(); // ������
    virtual ~Light(); // �Ҹ���

    virtual void FinalUpdate() override; // ���� ������Ʈ �Լ�
    void Render(); // ������ �Լ�
    void RenderShadow(); // �׸��� ������ �Լ�

public:

    // ����Ʈ Ÿ���� �������� �Լ�
    LIGHT_TYPE GetLightType() { return static_cast<LIGHT_TYPE>(_lightInfo.lightType); }

    // ����Ʈ ������ �������� �Լ�
    const LightInfo& GetLightInfo() { return _lightInfo; }

    // ����Ʈ ������ �����ϴ� �Լ�
    void SetLightDirection(Vec3 direction);

    // ����Ʈ�� ������ �����ϴ� �Լ���
    void SetDiffuse(const Vec3& diffuse);
    void SetAmbient(const Vec3& ambient);
    void SetSpecular(const Vec3& specular);

    // ����Ʈ Ÿ���� �����ϴ� �Լ�
    void SetLightType(LIGHT_TYPE type);

    // ����Ʈ ������ �����ϴ� �Լ�
    void SetLightRange(float range);

    // ����Ʈ ������ �����ϴ� �Լ�
    void SetLightAngle(float angle);

    // ����Ʈ �ε����� �����ϴ� �Լ�
    void SetLightIndex(int8 index);

private:
    LightInfo _lightInfo = {}; // ����Ʈ ����

    int8 _lightIndex = -1; // ����Ʈ �ε���
    shared_ptr<class Mesh> _volumeMesh; // ���� �޽�
    shared_ptr<class Material> _lightMaterial; // ����Ʈ ����

    shared_ptr<GameObject> _shadowCamera; // �׸��� ī�޶�
};