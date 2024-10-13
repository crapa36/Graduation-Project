#include "pch.h"
#include "TestReflection.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Camera.h"
#include "Transform.h"

TestReflection::TestReflection()
{
}

TestReflection::~TestReflection()
{
}

void TestReflection::LateUpdate()
{
	shared_ptr<Scene> scene = GET_SINGLETON(SceneManager)->GetActiveScene();
	shared_ptr<Transform> parent = GetTransform()->GetParent().lock();
	shared_ptr<Transform> cameraParent = scene->GetMainCamera()->GetTransform()->GetParent().lock();

	Vec3 cameraParentPos = cameraParent->GetTransform()->GetLocalPosition();
	Vec3 cameraRevolution = scene->GetMainCamera()->GetTransform()->GetLocalRevolution();
	Vec3 parentPos = parent->GetTransform()->GetLocalPosition();
	Vec3 pos = GetTransform()->GetLocalPosition();
	Vec3 revolution = GetTransform()->GetLocalRotation();

	pos = cameraParentPos;
	pos.y = parentPos.y - (cameraParentPos.y - parentPos.y);

	revolution = cameraRevolution;
	revolution.x = -cameraRevolution.x;

	GetTransform()->SetLocalPosition(pos);
	GetTransform()->SetLocalRevolution(revolution);

}
