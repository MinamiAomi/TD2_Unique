#include "DefaultTextures.h"

#include "Core/Graphics.h"
#include "Core/CommandContext.h"

namespace DefaultTexture{

    TextureResource White;
    TextureResource Black;
    TextureResource Normal;

    void Initialize() {

        CommandContext commandContext;
        commandContext.Start(D3D12_COMMAND_LIST_TYPE_DIRECT);
        UINT white = 0xFFFFFFFF;
        White.Create(commandContext, 1, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, &white);
        UINT black = 0xFF000000;
        Black.Create(commandContext, 1, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, &black);
        UINT normal = 0xFFFF8080;
        Normal.Create(commandContext, 1, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, &normal);
        commandContext.Finish(true);
    }

    void Finalize() {
        White.Destroy();
        Black.Destroy();
        Normal.Destroy();
    }
}

