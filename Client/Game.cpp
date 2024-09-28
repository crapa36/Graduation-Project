#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Network.h"

void Game::Init(const WindowInfo& info) {
    if (!initialize_winsock()) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
    }

    if (!connect_to_server("127.0.0.1")) {
    }

    send_login_packet("PlayerName");
    GEngine->Init(info);
    
    //GET_SINGLETON(SceneManager)->LoadScene(L"TestScene");
    GET_SINGLETON(SceneManager)->LoadScene(L"TestMenuScene");
}

void Game::Update() {
    GEngine->Update();
}

