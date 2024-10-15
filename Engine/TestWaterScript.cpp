#include "pch.h"
#include "TestWaterScript.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Timer.h"

TestWaterScript::TestWaterScript()
{
}

TestWaterScript::~TestWaterScript()
{
}

void TestWaterScript::LateUpdate()
{
	if(GetMeshRenderer())
		GetMeshRenderer()->GetMaterial()->SetFloat(0, GET_SINGLETON(Timer)->GetTotalTime());
}
