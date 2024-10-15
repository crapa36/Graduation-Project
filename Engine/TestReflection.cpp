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
	shared_ptr<Transform> camera = scene->GetMainCamera()->GetTransform();

	Matrix cameraM = camera->GetLocalToWorldMatrix();
	Quaternion quaternion;
	Vec3 cameraPos, cameraRotation, cameraScale;

	DecomposeMatrix(cameraM, cameraPos, quaternion, cameraScale);

	cameraRotation = camera->GetLocalRevolution();
	cameraRotation.x = -cameraRotation.x;

	GetTransform()->SetLocalPosition(cameraPos);
	GetTransform()->SetLocalRotation(cameraRotation);
}
