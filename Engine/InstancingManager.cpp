#include "pch.h"
#include "InstancingManager.h"
#include "InstancingBuffer.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"

// 게임 오브젝트를 렌더링하는 함수입니다.
void InstancingManager::Render(vector<shared_ptr<GameObject>>& gameObjects) {

    // 인스턴스 ID를 키로 하고, 해당 ID를 공유하는 게임 오브젝트들의 리스트를 값으로 하는 캐시 맵입니다.
    map<uint64, vector<shared_ptr<GameObject>>> cache;

    // 모든 게임 오브젝트를 순회하며 캐시에 추가합니다.
    for (shared_ptr<GameObject>& gameObject : gameObjects) {

        // 게임 오브젝트의 메시 렌더러에서 인스턴스 ID를 가져옵니다.
        const uint64 instanceId = gameObject->GetMeshRenderer()->GetInstanceID();

        // 인스턴스 ID를 키로 하여 게임 오브젝트를 캐시에 추가합니다.
        cache[instanceId].push_back(gameObject);
    }

    // 캐시된 모든 게임 오브젝트 그룹을 순회합니다.
    for (auto& pair : cache) {
        const vector<shared_ptr<GameObject>>& vec = pair.second;

        // 그룹에 게임 오브젝트가 하나만 있는 경우, 단일 렌더링을 수행합니다.
        if (vec.size() == 1) {
            vec[0]->GetMeshRenderer()->Render();
        }
        else {

            // 그룹에 여러 게임 오브젝트가 있는 경우, 인스턴싱 렌더링을 준비합니다.
            const uint64 instanceId = pair.first;

            // 각 게임 오브젝트에 대해 인스턴싱 파라미터를 설정합니다.
            for (const shared_ptr<GameObject>& gameObject : vec) {
                InstancingParams params;

                // 월드, 뷰, 프로젝션 행렬을 계산합니다.
                params.matWorld = gameObject->GetTransform()->GetLocalToWorldMatrix();
                params.matWV = params.matWorld * Camera::S_MatView;
                params.matWVP = params.matWorld * Camera::S_MatView * Camera::S_MatProjection;

                // 인스턴싱 파라미터를 추가합니다.
                AddParam(instanceId, params);
            }

            // 인스턴싱 버퍼를 가져와서 렌더링을 수행합니다.
            shared_ptr<InstancingBuffer>& buffer = _buffers[instanceId];
            vec[0]->GetMeshRenderer()->Render(buffer);
        }
    }
}

// 모든 인스턴싱 버퍼를 클리어하는 함수입니다.
void InstancingManager::ClearBuffer() {
    for (auto& pair : _buffers) {
        shared_ptr<InstancingBuffer>& buffer = pair.second;
        buffer->Clear();
    }
}

// 인스턴싱 파라미터를 추가하는 함수입니다.
void InstancingManager::AddParam(uint64 instanceId, InstancingParams& data) {

    // 인스턴스 ID에 해당하는 버퍼가 없으면 새로 생성합니다.
    if (_buffers.find(instanceId) == _buffers.end())
        _buffers[instanceId] = make_shared<InstancingBuffer>();

    // 인스턴싱 데이터를 버퍼에 추가합니다.
    _buffers[instanceId]->AddData(data);
}