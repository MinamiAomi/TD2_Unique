#include <Windows.h>

#include "Framework/Engine.h"
#include "AL4Game.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

    Game* game = new AL4Game;
    Engine::Run(game);
    delete game;

    return 0;
}