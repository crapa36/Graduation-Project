#pragma once
#include "Component.h"

class Material;
class Mesh;
class StructuredBuffer;

struct ParticleInfo {
    Vec3	worldPos;
    float	curTime;
    Vec3	worldDir;
    float	lifeTime;
    int32	alive;
    int32	padding[3];
};

struct ComputeSharedInfo {
    int32 addCount;
    int32 padding[3];
};

class ParticleSystem : public Component {
public:
    ParticleSystem();
    virtual ~ParticleSystem();

public:
    virtual void FinalUpdate();
    void Render();
    void SetTexture(const wstring& texturePath);
    void SetMaxParticle(uint32 maxParticle) { _maxParticle = maxParticle; }
    void SetCreateInterval(float interval) { _createInterval = interval; }
    void SetLifeTime(float min, float max) { _minLifeTime = min; _maxLifeTime = max; }
    void SetSpeed(float min, float max) { _minSpeed = min; _maxSpeed = max; }
    void SetScale(float start, float end) { _startScale = start; _endScale = end; }
    void SetLoop(bool loop) { _isLoop = loop; }
    void SetPlayTime(float time) { _playTime = time; }

public:
    virtual void Load(const wstring& path) override {}
    virtual void Save(const wstring& path) override {}

    wstring GetPath() { return _path; }

    uint32 GetMaxParticle() { return _maxParticle; }

    float GetCreateInterval() { return _createInterval; }
    float GetMinLifeTime() { return _minLifeTime; }
    float GetMaxLifeTime() { return _maxLifeTime; }
    float GetMinSpeed() { return _minSpeed; }
    float GetMaxSpeed() { return _maxSpeed; }
    float GetStartScale() { return _startScale; }
    float GetEndScale() { return _endScale; }

private:
    wstring                         _path;

    shared_ptr<StructuredBuffer>	_particleBuffer;
    shared_ptr<StructuredBuffer>	_computeSharedBuffer;
    uint32							_maxParticle = 2000;

    shared_ptr<Material>		_computeMaterial;
    shared_ptr<Material>		_material;
    shared_ptr<Mesh>			_mesh;

    float				_createInterval = 0.002f;
    float				_accTime = 0.f;

    float				_minLifeTime = 0.2f;
    float				_maxLifeTime = 0.4f;
    float				_minSpeed = 200;
    float				_maxSpeed = 100;
    float				_startScale = 20.f;
    float				_endScale = 5.f;

    bool				_isLoop = true;

    float                _playTime = 0.f;
};
