#pragma once

#ifdef _DEBUG
#include "Externals/ImGui/imgui.h"
#endif // _DEBUG


#include <Windows.h>
#include <d3d12.h>

#include "Core/DescriptorHandle.h"

class CommandContext;

class ImGuiManager {
public:
    static ImGuiManager* GetInstance();

    void Initialize(HWND hWnd, DXGI_FORMAT rtvFormat);
    void NewFrame();
    void Render(CommandContext& commandContext);
    void Shutdown();

private:
    ImGuiManager() = default;
    ImGuiManager(const ImGuiManager&) = delete;
    ImGuiManager& operator=(const ImGuiManager&) = delete;

    DescriptorHandle descriptor_;
};