#include "ImGuiManager.h"

#ifdef _DEBUG
#include "Externals/ImGui/imgui_impl_dx12.h"
#include "Externals/ImGui/imgui_impl_win32.h"
#endif // _DEBUG


#include "Core/Graphics.h"
#include "Core/SwapChain.h"
#include "Core/CommandContext.h"

ImGuiManager* ImGuiManager::GetInstance() {
    static ImGuiManager instance;
    return &instance;
}

void ImGuiManager::Initialize(HWND hWnd, DXGI_FORMAT rtvFormat) {
#ifdef _DEBUG
    auto graphics = Graphics::GetInstance();
    descriptor_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX12_Init(
        graphics->GetDevice(),
        SwapChain::kNumBuffers,
        rtvFormat,
        graphics->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
        descriptor_,
        descriptor_);
#else
    hWnd; rtvFormat;
#endif _DEBUG
}

void ImGuiManager::NewFrame() {
#ifdef _DEBUG
    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX12_NewFrame();
    ImGui::NewFrame();
#endif _DEBUG
}

void ImGuiManager::Render(CommandContext& commandContext) {
#ifdef _DEBUG
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandContext);
#else
    commandContext;
#endif
}

void ImGuiManager::Shutdown() {
#ifdef _DEBUG
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
#endif // _DEBUG

}
