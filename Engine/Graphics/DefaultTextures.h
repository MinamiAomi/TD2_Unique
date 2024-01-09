#pragma once

#include "Core/TextureResource.h"

namespace DefaultTexture {

    extern TextureResource White;
    extern TextureResource Black;
    extern TextureResource Normal;

    void Initialize();
    void Finalize();

}