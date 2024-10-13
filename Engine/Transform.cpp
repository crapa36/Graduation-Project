#include "pch.h"
#include "Transform.h"
#include "Engine.h"
#include "Camera.h"
#include "Timer.h"

Transform::Transform() : Component(COMPONENT_TYPE::TRANSFORM) {
}

Transform::~Transform() {
}

void Transform::FinalUpdate() {
    _localQuaternionRotation = QuaternionToEuler(_quaternion);
    Matrix matScale = Matrix::CreateScale(_localScale);
    Matrix matRotation = Matrix::CreateRotationX(_localRotation.x + _localQuaternionRotation.x);
    matRotation *= Matrix::CreateRotationY(_localRotation.y + _localQuaternionRotation.y);
    matRotation *= Matrix::CreateRotationZ(_localRotation.z + _localQuaternionRotation.z);
    Matrix matTranslation = Matrix::CreateTranslation(_localPosition);
    Matrix matRevolution = Matrix::CreateRotationX(_localRevolution.x);
    matRevolution *= Matrix::CreateRotationY(_localRevolution.y);
    matRevolution *= Matrix::CreateRotationZ(_localRevolution.z);

    _matLocal = matScale * matRotation * matTranslation * matRevolution;
    _matWorld = _matLocal;

    shared_ptr<Transform> parent = GetParent().lock();
    if (parent != nullptr) {
        if (_inheritScale) {
            Matrix parentmatScale = Matrix::CreateScale(parent->GetLocalScale());
            _matWorld *= parentmatScale;
        }
        if (_inheritRotation) {
            Matrix parentMatRotation = Matrix::CreateRotationX(parent->GetLocalRotation().x);
            parentMatRotation *= Matrix::CreateRotationY(parent->GetLocalRotation().y);
            parentMatRotation *= Matrix::CreateRotationZ(parent->GetLocalRotation().z);
            _matWorld *= parentMatRotation;
        }
        if (_inheritPosition) {
            Matrix parentMatTranslation = Matrix::CreateTranslation(parent->GetLocalPosition());
            _matWorld *= parentMatTranslation;
        }
        if (_inheritRevolution) {
            Matrix parentMatRevolution = Matrix::CreateRotationX(parent->GetLocalRevolution().x);
            parentMatRevolution *= Matrix::CreateRotationY(parent->GetLocalRevolution().y);
            parentMatRevolution *= Matrix::CreateRotationZ(parent->GetLocalRevolution().z);
            _matWorld *= parentMatRevolution;
        }
    }
}

void Transform::PushData() {
    TransformParams transformParams = {};
    transformParams.matWorld = _matWorld;
    transformParams.matView = Camera::S_MatView;
    transformParams.matProjection = Camera::S_MatProjection;
    transformParams.matWV = _matWorld * Camera::S_MatView;
    transformParams.matWVP = _matWorld * Camera::S_MatView * Camera::S_MatProjection;
    transformParams.matViewInv = Camera::S_MatView.Invert();
    transformParams.matProjectionInv = Camera::S_MatProjection.Invert();

    CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushGraphicsData(&transformParams, sizeof(transformParams));
}

void Transform::LookAt(const Vec3& dir) {
    Vec3 front = dir;
    front.Normalize();

    Vec3 right = Vec3::Up.Cross(dir);
    if (right == Vec3::Zero)
        right = Vec3::Forward.Cross(dir);

    right.Normalize();

    Vec3 up = front.Cross(right);
    up.Normalize();

    Matrix matrix = XMMatrixIdentity();
    matrix.Right(right);
    matrix.Up(up);
    matrix.Backward(front);

    _localRotation = DecomposeRotationMatrix(matrix);
}

bool Transform::CloseEnough(const float& a, const float& b, const float& epsilon) {
    return (epsilon > std::abs(a - b));
}

Vec3 Transform::DecomposeRotationMatrix(const Matrix& rotation) {
    Vec4 v[4];
    XMStoreFloat4(&v[0], rotation.Right());
    XMStoreFloat4(&v[1], rotation.Up());
    XMStoreFloat4(&v[2], rotation.Backward());
    XMStoreFloat4(&v[3], rotation.Translation());

    Vec3 ret;
    if (CloseEnough(v[0].z, -1.0f)) {
        float x = 0;
        float y = XM_PI / 2;
        float z = x + atan2(v[1].x, v[2].x);
        ret = Vec3{ x, y, z };
    }
    else if (CloseEnough(v[0].z, 1.0f)) {
        float x = 0;
        float y = -XM_PI / 2;
        float z = -x + atan2(-v[1].x, -v[2].x);
        ret = Vec3{ x, y, z };
    }
    else {
        float y1 = -asin(v[0].z);
        float y2 = XM_PI - y1;

        float x1 = atan2f(v[1].z / cos(y1), v[2].z / cos(y1));
        float x2 = atan2f(v[1].z / cos(y2), v[2].z / cos(y2));

        float z1 = atan2f(v[0].y / cos(y1), v[0].x / cos(y1));
        float z2 = atan2f(v[0].y / cos(y2), v[0].x / cos(y2));

        if ((std::abs(x1) + std::abs(y1) + std::abs(z1)) <= (std::abs(x2) + std::abs(y2) + std::abs(z2))) {
            ret = Vec3{ x1, y1, z1 };
        }
        else {
            ret = Vec3{ x2, y2, z2 };
        }
    }

    return ret;
}

Vec3 Transform::QuaternionToEuler(const Quaternion& q)
{
    // 쿼터니언을 행렬로 변환
    XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(q);

    // 행렬의 요소 추출
    float pitch, yaw, roll;

    // 행렬의 요소를 사용하여 오일러 각 계산
    // Pitch (X-axis rotation)
    pitch = std::asin(-rotationMatrix.r[2].m128_f32[1]);

    // Roll (Z-axis rotation)
    if (std::cos(pitch) > 0.0001f) { // 안정성을 위한 작은 수 비교
        roll = std::atan2(rotationMatrix.r[2].m128_f32[0], rotationMatrix.r[2].m128_f32[2]);
        yaw = std::atan2(rotationMatrix.r[0].m128_f32[1], rotationMatrix.r[1].m128_f32[1]);
    }
    else {
        roll = std::atan2(-rotationMatrix.r[1].m128_f32[0], rotationMatrix.r[0].m128_f32[0]);
        yaw = 0.0f;
    }

    // 오일러 각을 Vector3로 반환 (Yaw, Pitch, Roll)
    return Vector3(pitch, yaw, roll); // 피치, 요, 롤 순으로 반환
}