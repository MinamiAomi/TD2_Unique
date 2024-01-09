#pragma once

class BaseScene {
public:
    virtual ~BaseScene() {}

    virtual void OnInitialize() = 0 {}
    virtual void OnUpdate() = 0 {}
    virtual void OnFinalize() = 0 {}
};