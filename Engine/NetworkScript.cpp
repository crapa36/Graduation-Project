#include "pch.h"
#include "NetworkScript.h"
#include "NetworkManager.h"

NetworkScript::NetworkScript(){}

NetworkScript::~NetworkScript(){}

void NetworkScript::Awake()
{
	// 여기다가 패킷 받아서 추가 
	GET_SINGLETON(NetworkManager)->send_login_packet();
}

void NetworkScript::Update()
{
	// 비동기 recv
	GET_SINGLETON(NetworkManager)->Update();

}
