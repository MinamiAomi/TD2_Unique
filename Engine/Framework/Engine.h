#pragma once

#include <memory>

#include "Input/Input.h"

class Game;

class Engine {
public:
    static void Run(Game* game);
};