#pragma once

// std::byte 사용하지 않음
#define _HAS_STD_BYTE 0


#define PIE 3.1415

// 각종 include
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <ws2tcpip.h>

#include <windows.h>
#include <iostream>
#include <fstream>
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
using namespace DirectX::SimpleMath;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>
#include "DDSTextureLoader12.h"
#include "WICTextureLoader12.h"

#include "FBX/fbxsdk.h"

// 각종 lib
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dinput8")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_debug.lib")
#else
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "FBX\\debug\\libfbxsdk-md.lib")
#pragma comment(lib, "FBX\\debug\\libxml2-md.lib")
#pragma comment(lib, "FBX\\debug\\zlib-md.lib")
#else
#pragma comment(lib, "FBX\\release\\libfbxsdk-md.lib")
#pragma comment(lib, "FBX\\release\\libxml2-md.lib")
#pragma comment(lib, "FBX\\release\\zlib-md.lib")
#endif



// 각종 typedef
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
    t5,
    t6,
    t7,
    t8,
    t9,

    END
};

enum class UAV_REGISTER : uint8 {
    u0 = static_cast<uint8>(SRV_REGISTER::END),
    u1,
    u2,
    u3,
    u4,

    END
};

enum {
    SWAP_CHAIN_BUFFER_COUNT = 2,
    CBV_REGISTER_COUNT = CBV_REGISTER::END,
    SRV_REGISTER_COUNT = static_cast<uint8>(SRV_REGISTER::END) - CBV_REGISTER_COUNT,
    CBV_SRV_REGISTER_COUNT = CBV_REGISTER_COUNT + SRV_REGISTER_COUNT,
    UAV_REGISTER_COUNT = static_cast<uint8>(UAV_REGISTER::END) - CBV_SRV_REGISTER_COUNT,
    TOTAL_REGISTER_COUNT = CBV_SRV_REGISTER_COUNT + UAV_REGISTER_COUNT,
};

struct WindowInfo {
    HWND	hwnd; // 출력 윈도우
    int32	width; // 너비
    int32	height; // 높이
    int32   clientWidth; // 클라이언트 영역 너비
    int32   clientHeight; // 클라이언트 영역 높이
    bool	windowed; // 창모드 or 전체화면
    HINSTANCE hInstance; // 인스턴스
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
    Vec4 weights;
    Vec4 indices;
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

#define GET_SINGLETON(type)         type::GetInstance()

#define DEVICE				        GEngine->GetDevice()->GetDevice()
#define GRAPHICS_CMD_LIST	        GEngine->GetGraphicsCmdQueue()->GetGraphicsCmdList()
#define COMPUTE_CMD_LIST	        GEngine->GetComputeCmdQueue()->GetComputeCmdList()
#define RESOURCE_CMD_LIST	        GEngine->GetGraphicsCmdQueue()->GetResourceCmdList()
#define GRAPHICS_ROOT_SIGNATURE		GEngine->GetRootSignature()->GetGraphicsRootSignature()
#define COMPUTE_ROOT_SIGNATURE		GEngine->GetRootSignature()->GetComputeRootSignature()

#define INPUT				        GET_SINGLETON(Input)
#define DELTA_TIME			        GET_SINGLETON(Timer)->GetDeltaTime()

#define CONST_BUFFER(type)	        GEngine->GetConstantBuffer(type)

struct TransformParams {
    Matrix matWorld;
    Matrix matView;
    Matrix matProjection;
    Matrix matWV;
    Matrix matWVP;
    Matrix matViewInv;
};

struct AnimFrameParams {
    Vec4 scale;
    Vec4 rotation;
    Vec4 translate;
};

extern unique_ptr<class Engine> GEngine;

wstring s2ws(const string& str);
string ws2s(const wstring& wstr);