#pragma once
#include "Object.h"

class Shader;
class Texture;
class CubeMapTexture;

enum {
    MATERIAL_ARG_COUNT = 4,
};

enum {
    MAX_TEXTURES = 8,
};

struct MaterialParams {
    MaterialParams() {
        intParams.fill(0);
        floatParams.fill(0.0f);
        vec2Params.fill(Vec2(0.0f, 0.0f));
        vec4Params.fill(Vec4(0.0f, 0.0f, 0.0f, 0.0f));
        matrixParams.fill(Matrix());
        texOnParams.fill(0);
    }
    void SetInt(uint8 index, int32 value) { intParams[index] = value; }
    void SetFloat(uint8 index, float value) { floatParams[index] = value; }
    void SetVec2(uint8 index, Vec2 value) { vec2Params[index] = value; }
    void SetVec4(uint8 index, Vec4 value) { vec4Params[index] = value; }
    void SetMatrix(uint8 index, Matrix& value) { matrixParams[index] = value; }
    void SetTexOn(uint8 index, int32 value) { texOnParams[index] = value; }

    array<int32, MATERIAL_ARG_COUNT> intParams;
    array<float, MATERIAL_ARG_COUNT> floatParams;
    array<Vec2, MATERIAL_ARG_COUNT> vec2Params;
    array<Vec4, MATERIAL_ARG_COUNT> vec4Params;
    array<Matrix, MATERIAL_ARG_COUNT> matrixParams;
    array<int32, MATERIAL_ARG_COUNT * 2> texOnParams;
};

class Material : public Object {
public:
    Material();
    virtual ~Material();

    shared_ptr<Shader> GetShader() { return _shader; }

    void SetShader(shared_ptr<Shader> shader) { _shader = shader; }
    void SetInt(uint8 index, int32 value) { _params.SetInt(index, value); }
    void SetFloat(uint8 index, float value) { _params.SetFloat(index, value); }
    void SetTexture(uint8 index, shared_ptr<Texture> texture) {
        _textures[index] = texture;
        _params.SetTexOn(index, (texture == nullptr ? 0 : 1));
    }
    void SetCubeMapTexture(shared_ptr<CubeMapTexture> texture) { _cubeMapTexture = texture; }

    void SetVec2(uint8 index, Vec2 value) { _params.SetVec2(index, value); }
    void SetVec4(uint8 index, Vec4 value) { _params.SetVec4(index, value); }
    void SetMatrix(uint8 index, const Matrix& value) { _params.matrixParams[index] = value; }

    void PushGraphicsData();
    void PushComputeData();
    void Dispatch(uint32 x, uint32 y, uint32 z);

    shared_ptr<Material> Clone();

    array<shared_ptr<Texture>, MATERIAL_ARG_COUNT> GetTextures() { return _textures; }
    shared_ptr<Texture> GetTexture(uint32 i) { return _textures[i]; }

private:
    shared_ptr<Shader>	_shader;
    MaterialParams		_params;
    array<shared_ptr<Texture>, MATERIAL_ARG_COUNT> _textures;
    shared_ptr<CubeMapTexture> _cubeMapTexture;
};
