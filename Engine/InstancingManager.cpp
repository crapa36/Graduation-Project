#include "pch.h"
#include "InstancingManager.h"
#include "InstancingBuffer.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"

// ���� ������Ʈ�� �������ϴ� �Լ��Դϴ�.
void InstancingManager::Render(vector<shared_ptr<GameObject>>& gameObjects) {

    // �ν��Ͻ� ID�� Ű�� �ϰ�, �ش� ID�� �����ϴ� ���� ������Ʈ���� ����Ʈ�� ������ �ϴ� ĳ�� ���Դϴ�.
    map<uint64, vector<shared_ptr<GameObject>>> cache;

    // ��� ���� ������Ʈ�� ��ȸ�ϸ� ĳ�ÿ� �߰��մϴ�.
    for (shared_ptr<GameObject>& gameObject : gameObjects) {

        // ���� ������Ʈ�� �޽� ���������� �ν��Ͻ� ID�� �����ɴϴ�.
        const uint64 instanceId = gameObject->GetMeshRenderer()->GetInstanceID();

        // �ν��Ͻ� ID�� Ű�� �Ͽ� ���� ������Ʈ�� ĳ�ÿ� �߰��մϴ�.
        cache[instanceId].push_back(gameObject);
    }

    // ĳ�õ� ��� ���� ������Ʈ �׷��� ��ȸ�մϴ�.
    for (auto& pair : cache) {
        const vector<shared_ptr<GameObject>>& vec = pair.second;

        // �׷쿡 ���� ������Ʈ�� �ϳ��� �ִ� ���, ���� �������� �����մϴ�.
        if (vec.size() == 1) {
            vec[0]->GetMeshRenderer()->Render();
        }
        else {

            // �׷쿡 ���� ���� ������Ʈ�� �ִ� ���, �ν��Ͻ� �������� �غ��մϴ�.
            const uint64 instanceId = pair.first;

            // �� ���� ������Ʈ�� ���� �ν��Ͻ� �Ķ���͸� �����մϴ�.
            for (const shared_ptr<GameObject>& gameObject : vec) {
                InstancingParams params;

                // ����, ��, �������� ����� ����մϴ�.
                params.matWorld = gameObject->GetTransform()->GetLocalToWorldMatrix();
                params.matWV = params.matWorld * Camera::S_MatView;
                params.matWVP = params.matWorld * Camera::S_MatView * Camera::S_MatProjection;

                // �ν��Ͻ� �Ķ���͸� �߰��մϴ�.
                AddParam(instanceId, params);
            }

            // �ν��Ͻ� ���۸� �����ͼ� �������� �����մϴ�.
            shared_ptr<InstancingBuffer>& buffer = _buffers[instanceId];
            vec[0]->GetMeshRenderer()->Render(buffer);
        }
    }
}

// ��� �ν��Ͻ� ���۸� Ŭ�����ϴ� �Լ��Դϴ�.
void InstancingManager::ClearBuffer() {
    for (auto& pair : _buffers) {
        shared_ptr<InstancingBuffer>& buffer = pair.second;
        buffer->Clear();
    }
}

// �ν��Ͻ� �Ķ���͸� �߰��ϴ� �Լ��Դϴ�.
void InstancingManager::AddParam(uint64 instanceId, InstancingParams& data) {

    // �ν��Ͻ� ID�� �ش��ϴ� ���۰� ������ ���� �����մϴ�.
    if (_buffers.find(instanceId) == _buffers.end())
        _buffers[instanceId] = make_shared<InstancingBuffer>();

    // �ν��Ͻ� �����͸� ���ۿ� �߰��մϴ�.
    _buffers[instanceId]->AddData(data);
}