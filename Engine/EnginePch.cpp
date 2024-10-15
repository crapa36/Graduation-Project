#include "pch.h"
#include "EnginePch.h"
#include "Engine.h"

unique_ptr<Engine> GEngine = make_unique<Engine>();

wstring s2ws(const string& s) {
    int32 len;
    int32 slength = static_cast<int32>(s.length()) + 1;
    len = ::MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    ::MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    wstring ret(buf);
    delete[] buf;
    return ret;
}

string ws2s(const wstring& s) {
    int32 len;
    int32 slength = static_cast<int32>(s.length()) + 1;
    len = ::WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
    string r(len, '\0');
    ::WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
    return r;
}

bool IsMatrixValid(const XMFLOAT4X4& matrix)
{
    // XMFLOAT4X4의 모든 요소가 0이 아닐 경우 유효하다고 간주
    return matrix._11 != 0 || matrix._12 != 0 || matrix._13 != 0 || matrix._14 != 0 ||
        matrix._21 != 0 || matrix._22 != 0 || matrix._23 != 0 || matrix._24 != 0 ||
        matrix._31 != 0 || matrix._32 != 0 || matrix._33 != 0 || matrix._34 != 0 ||
        matrix._41 != 0 || matrix._42 != 0 || matrix._43 != 0 || matrix._44 != 0;
}

void DecomposeMatrix(const XMFLOAT4X4& matrix, Vec3& position, Quaternion& rotation, Vec3& scale)
{
    // 4x4 행렬을 XMMATRIX로 변환
    XMMATRIX mat = XMLoadFloat4x4(&matrix);

    // 위치, 회전, 스케일 벡터를 초기화
    XMVECTOR posVec, rotQuat, sclVec;

    // 행렬을 위치, 회전(Quaternion), 스케일로 분리
    XMMatrixDecompose(&sclVec, &rotQuat, &posVec, mat);

    // 분리된 값들을 SimpleMath의 Vector3와 Quaternion 형식으로 변환
    position = Vector3(posVec);
    rotation = Quaternion(rotQuat);
    scale = Vector3(sclVec);
}

Vec3 QuaternionToEuler(const Quaternion& q)
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