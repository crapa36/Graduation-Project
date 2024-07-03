#pragma once

#define _HAS_STD_BYTE 0

// ���� include
#include <windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
using namespace std;

#include <filesystem>
namespace fs = std::filesystem;

#include "d3dx12.h"
#include "SimpleMath.h"
#include <d3d12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>

// ���� lib
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")

#ifdef _DEBUG //���� �ʿ�
#pragma comment(lib, "DirectXTex_Debug")
#else
#pragma comment(lib, "DirectXTex")
#endif

// ���� typedef
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
using Vec2 = DirectX::SimpleMath::Vector2;
using Vec3 = DirectX::SimpleMath::Vector3;
using Vec4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;

enum class CBV_REGISTER : uint8 {
    b0,
    b1,
    b2,
    b3,
    b4,

    END
};

enum class SRV_REGISTER : uint8 {
    t0 = static_cast<uint8>(CBV_REGISTER::END),
    t1,
    t2,
    t3,
    t4,

    END
};

enum {
    SWAP_CHAIN_BUFFER_COUNT = 2,
    CBV_REGISTER_COUNT = CBV_REGISTER::END,
    SRV_REGISTER_COUNT = static_cast<uint8>(SRV_REGISTER::END) - CBV_REGISTER_COUNT,
    REGISTER_COUNT = CBV_REGISTER_COUNT + SRV_REGISTER_COUNT,
};

struct WindowInfo {
    HWND	hwnd; // ��� ������
    int32	width; // �ʺ�
    int32	height; // ����
    bool	windowed; // â��� or ��üȭ��
};

struct Vertex {
    Vertex() {}
    Vertex(Vec3 pos, Vec2 uv, Vec3 normal, Vec3 tangent)
        : pos(pos), uv(uv), normal(normal), tangent(tangent) {
    }
    Vec3 pos;
    Vec2 uv;
    Vec3 normal;
    Vec3 tangent;
};

#define DECLARE_SINGLETON(type)         \
private:                                \
    type() {}                           \
    ~type() {}                          \
public:                                 \
    static type* GetInstance() {        \
        static type instance;           \
        return &instance;               \
    }                                   \

#define GET_SINGLETON(type)    type::GetInstance()

#define DEVICE				GEngine->GetDevice()->GetDevice()
#define CMD_LIST			GEngine->GetCmdQueue()->GetCmdList()
#define RESOURCE_CMD_LIST	GEngine->GetCmdQueue()->GetResourceCmdList()
#define ROOT_SIGNATURE		GEngine->GetRootSignature()->GetSignature()

#define INPUT				GET_SINGLETON(Input)
#define DELTA_TIME			GET_SINGLETON(Timer)->GetDeltaTime()

#define CONST_BUFFER(type)	GEngine->GetConstantBuffer(type)

struct TransformParams {
    Matrix matWorld;
    Matrix matView;
    Matrix matProjection;
    Matrix matWV;
    Matrix matWVP;
};

extern unique_ptr<class Engine> GEngine;