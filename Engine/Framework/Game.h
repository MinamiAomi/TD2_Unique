#pragma once

class Game {
public:
    virtual ~Game() {}

    virtual void OnInitialize() = 0;
    virtual void OnFinalize() = 0;
};