#include "pch.h"
#include "NetworkScript.h"
#include "NetworkManager.h"

NetworkScript::NetworkScript(){}

NetworkScript::~NetworkScript(){}

void NetworkScript::Awake()
{
	// ����ٰ� ��Ŷ �޾Ƽ� �߰� 
	GET_SINGLETON(NetworkManager)->send_login_packet();
}

void NetworkScript::Update()
{
	// �񵿱� recv
	GET_SINGLETON(NetworkManager)->Update();

}
