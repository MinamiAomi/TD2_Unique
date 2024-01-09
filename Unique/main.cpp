#include <Windows.h>

#include "Framework/Engine.h"
#include "Unique.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

    Game* game = new Unique;
    Engine::Run(game);
    delete game;

    return 0;
}