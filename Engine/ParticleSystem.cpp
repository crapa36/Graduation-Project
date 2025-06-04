#include "pch.h"
#include "ParticleSystem.h"
#include "StructuredBuffer.h"
#include "Mesh.h"
#include "Resources.h"
#include "Transform.h"
#include "Timer.h"

ParticleSystem::ParticleSystem() : Component(COMPONENT_TYPE::PARTICLE_SYSTEM) {
    _particleBuffer = make_shared<StructuredBuffer>();
    _particleBuffer->Init(sizeof(ParticleInfo), _maxParticle);

    _computeSharedBuffer = make_shared<StructuredBuffer>();
    _computeSharedBuffer->Init(sizeof(ComputeSharedInfo), 1);

    _mesh = GResources->LoadPointMesh();
    _material = GResources->Get<Material>(L"Particle");
    shared_ptr<Texture> tex = GResources->Load<Texture>(
        L"Bubbles", L"..\\Resources\\Texture\\Particle\\spark.png");

    _material->SetTexture(0, tex);

    _computeMaterial = GResources->Get<Material>(L"ComputeParticle");
}

ParticleSystem::~ParticleSystem() {
}
void ParticleSystem::SetTexture(const wstring& texturePath) {
    shared_ptr<Texture> texture = GResources->Load<Texture>(L"ParticleTexture", texturePath);
    _material->SetTexture(0, texture);
}

void ParticleSystem::FinalUpdate() {
    _accTime += DELTA_TIME;

    int32 add = 0;

    if (_createInterval < _accTime) {
        int iterations = static_cast<int>(_accTime / _createInterval);
        _accTime -= iterations * _createInterval;
        add += iterations;
    }

    if (!_isLoop) {
        _playTime -= DELTA_TIME;
        if (_playTime <= 0) {
            add = 0;
        }
    }

    _particleBuffer->PushComputeUAVData(UAV_REGISTER::u0);
    _computeSharedBuffer->PushComputeUAVData(UAV_REGISTER::u1);

    _computeMaterial->SetInt(0, _maxParticle);
    _computeMaterial->SetInt(1, add);

    _computeMaterial->SetVec2(1, Vec2(DELTA_TIME, _accTime));
    _computeMaterial->SetVec4(0, Vec4(_minLifeTime, _maxLifeTime, _minSpeed, _maxSpeed));

    _computeMaterial->Dispatch(1, 1, 1);
}

void ParticleSystem::Render() {
    GetTransform()->PushData();

    _particleBuffer->PushGraphicsData(SRV_REGISTER::t9);
    _material->SetFloat(0, _startScale);
    _material->SetFloat(1, _endScale);
    _material->PushGraphicsData();

    _mesh->Render(_maxParticle);
}
