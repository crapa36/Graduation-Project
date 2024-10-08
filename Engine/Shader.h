#pragma once
#include "Object.h"

enum class SHADER_TYPE : uint8 {
    DEFERRED,
    FORWARD,
    LIGHTING,
    PARTICLE,
    COMPUTE,
    SHADOW,
};

enum class RASTERIZER_TYPE : uint8 {
    CULL_NONE,
    CULL_FRONT,
    CULL_BACK,
    WIREFRAME,
};

enum class DEPTH_STENCIL_TYPE : uint8 {
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    NO_DEPTH_TEST,          // ���� �׽�Ʈ(x) + ���� ���(o)
    NO_DEPTH_TEST_NO_WRITE, // ���� �׽�Ʈ(x) + ���� ���(x)
    LESS_EQUAL_NO_WRITE,
    LESS_NO_WRITE,          // ���� �׽�Ʈ(o) + ���� ���(x)
};

enum class BLEND_TYPE : uint8 {
    DEFAULT,
    ALPHA_BLEND,
    ONE_TO_ONE_BLEND,
    END,
};

struct ShaderInfo {
    SHADER_TYPE shaderType = SHADER_TYPE::FORWARD;
    RASTERIZER_TYPE rasterizerType = RASTERIZER_TYPE::CULL_BACK;
    DEPTH_STENCIL_TYPE depthStencilType = DEPTH_STENCIL_TYPE::LESS;
    BLEND_TYPE blendType = BLEND_TYPE::DEFAULT;
    D3D_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};

struct ShaderArg {
    const string vs = "VS_Main";
    const string hs;
    const string ds;
    const string gs;
    const string ps = "PS_Main";
};

class Shader : public Object {
public:
    Shader();
    virtual ~Shader();

    void CreateGraphicsShader(const wstring& path, ShaderInfo info = ShaderInfo(), ShaderArg arg = ShaderArg());
    void CreateGraphicsShader(ShaderInfo info = ShaderInfo());
    void CreateComputeShader(const wstring& path, const string& name, const string& version);
    void CreateComputeShader();

    void Update();

    SHADER_TYPE GetShaderType() { return _info.shaderType; }

    static D3D12_PRIMITIVE_TOPOLOGY_TYPE GetTopologyType(D3D_PRIMITIVE_TOPOLOGY topology);

    void SetVsBlob(ComPtr<ID3DBlob> Blob) { _vsBlob = Blob; }
    void SetHsBlob(ComPtr<ID3DBlob> Blob) { _hsBlob = Blob; }
    void SetDsBlob(ComPtr<ID3DBlob> Blob) { _dsBlob = Blob; }
    void SetGsBlob(ComPtr<ID3DBlob> Blob) { _gsBlob = Blob; }
    void SetPsBlob(ComPtr<ID3DBlob> Blob) { _psBlob = Blob; }
    void SetCsBlob(ComPtr<ID3DBlob> Blob){ _csBlob = Blob; }

    ComPtr<ID3DBlob> GetVsBlob() { return _vsBlob; }
    ComPtr<ID3DBlob> GetHsBlob() { return _hsBlob; }
    ComPtr<ID3DBlob> GetDsBlob() { return _dsBlob; }
    ComPtr<ID3DBlob> GetGsBlob() { return _gsBlob; }
    ComPtr<ID3DBlob> GetPsBlob() { return _psBlob; }
    ComPtr<ID3DBlob> GetCsBlob() { return _csBlob; }

    ShaderInfo GetInfo() { return _info; }

private:
    void CreateShader(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob, D3D12_SHADER_BYTECODE& shaderByteCode);
    void CreateVertexShader(const wstring& path, const string& name, const string& version);
    void CreateHullShader(const wstring& path, const string& name, const string& version);
    void CreateDomainShader(const wstring& path, const string& name, const string& version);
    void CreateGeometryShader(const wstring& path, const string& name, const string& version);
    void CreatePixelShader(const wstring& path, const string& name, const string& version);

private:
    ShaderInfo							_info;
    ComPtr<ID3D12PipelineState>			_pipelineState;

    // Graphics Shader
    ComPtr<ID3DBlob>					_vsBlob;
    ComPtr<ID3DBlob>					_hsBlob;
    ComPtr<ID3DBlob>					_dsBlob;
    ComPtr<ID3DBlob>					_gsBlob;
    ComPtr<ID3DBlob>					_psBlob;

    ComPtr<ID3DBlob>					_errBlob;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC  _graphicsPipelineDesc = {};

    // Compute Shader
    ComPtr<ID3DBlob>					_csBlob;
    D3D12_COMPUTE_PIPELINE_STATE_DESC   _computePipelineDesc = {};
};
