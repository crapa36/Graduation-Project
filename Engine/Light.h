#pragma once
#include "Component.h"

// 라이트의 타입을 정의하는 열거형입니다.
enum class LIGHT_TYPE : uint8 {
    DIRECTIONAL, // 방향성 라이트
    POINT,       // 포인트 라이트
    SPOT,        // 스폿 라이트
};

// 라이트의 색상 정보를 담는 구조체입니다.
struct LightColor {
    Vec4	diffuse;  // 확산 색상
    Vec4	ambient;  // 주변 색상
    Vec4	specular; // 반사 색상
};

// 라이트의 정보를 담는 구조체입니다.
struct LightInfo {
    LightColor	color;      // 라이트의 색상 정보
    Vec4		position;   // 라이트의 위치
    Vec4		direction;  // 라이트의 방향
    int32		lightType;  // 라이트의 타입
    float		range;      // 라이트의 범위
    float		angle;      // 라이트의 각도
    int32		padding;    // 패딩
};

// 라이트 파라미터를 담는 구조체입니다.
struct LightParams {
    uint32		lightCount; // 라이트의 개수
    Vec3		padding;    // 패딩
    LightInfo	lights[50]; // 라이트 정보 배열
};

// 라이트 컴포넌트 클래스입니다.
class Light : public Component {
public:
    Light(); // 생성자
    virtual ~Light(); // 소멸자

    virtual void FinalUpdate() override; // 최종 업데이트 함수
    void Render(); // 렌더링 함수
    void RenderShadow(); // 그림자 렌더링 함수

public:

    // 라이트 타입을 가져오는 함수
    LIGHT_TYPE GetLightType() { return static_cast<LIGHT_TYPE>(_lightInfo.lightType); }

    // 라이트 정보를 가져오는 함수
    const LightInfo& GetLightInfo() { return _lightInfo; }

    // 라이트 방향을 설정하는 함수
    void SetLightDirection(Vec3 direction);

    // 라이트의 색상을 설정하는 함수들
    void SetDiffuse(const Vec3& diffuse);
    void SetAmbient(const Vec3& ambient);
    void SetSpecular(const Vec3& specular);

    // 라이트 타입을 설정하는 함수
    void SetLightType(LIGHT_TYPE type);

    // 라이트 범위를 설정하는 함수
    void SetLightRange(float range);

    // 라이트 각도를 설정하는 함수
    void SetLightAngle(float angle);

    // 라이트 인덱스를 설정하는 함수
    void SetLightIndex(int8 index);

private:
    LightInfo _lightInfo = {}; // 라이트 정보

    int8 _lightIndex = -1; // 라이트 인덱스
    shared_ptr<class Mesh> _volumeMesh; // 볼륨 메시
    shared_ptr<class Material> _lightMaterial; // 라이트 재질

    shared_ptr<GameObject> _shadowCamera; // 그림자 카메라
};