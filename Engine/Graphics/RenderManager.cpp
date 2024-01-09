#include "RenderManager.h"

#include "Core/ShaderManager.h"
#include "DefaultTextures.h"
#include "GameWindow.h"
#include "ImGuiManager.h"

RenderManager* RenderManager::GetInstance() {
    static RenderManager instance;
    return &instance;
}

void RenderManager::Initialize() {
    graphics_ = Graphics::GetInstance();

    auto shaderManager = ShaderManager::GetInstance();
    shaderManager->Initialize();
    shaderManager->SetDirectory(std::filesystem::current_path() / SHADER_DIRECTORY);

    auto vs = shaderManager->Compile(L"Standard/GeometryPassVS.hlsl", ShaderType::Vertex, 6, 6);
    auto ps = shaderManager->Compile(L"Standard/GeometryPassPS.hlsl", ShaderType::Pixel, 6, 6);
    auto lps = shaderManager->Compile(L"Standard/LightingPassPS.hlsl", ShaderType::Pixel, 6, 6);
    auto rt = shaderManager->Compile(L"Raytracing/Raytracing.hlsl", ShaderType::Library, 6, 6);


    auto window = GameWindow::GetInstance();
    swapChain_.Create(window->GetHWND());

    DefaultTexture::Initialize();

    auto& swapChainBuffer = swapChain_.GetColorBuffer(0);
    float clearColor[4] = { 0.1f, 0.4f, 0.6f, 0.0f };
    mainColorBuffer_.SetClearColor(clearColor);
    mainColorBuffer_.Create(L"MainColorBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM);
    mainDepthBuffer_.Create(L"MainDepthBuffer", swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight(), DXGI_FORMAT_D32_FLOAT);

    toonRenderer_.Initialize(mainColorBuffer_, mainDepthBuffer_);
    particleRenderer_.Initialize(mainColorBuffer_, mainDepthBuffer_);
    postEffect_.Initialize(swapChainBuffer);
    spriteRenderer_.Initialize(swapChainBuffer);

    modelRenderer.Initialize(mainColorBuffer_, mainDepthBuffer_);
    raytracingRenderer_.Create(mainColorBuffer_.GetWidth(), mainColorBuffer_.GetHeight());
    raymarchingRenderer_.Create(mainColorBuffer_.GetWidth(), mainColorBuffer_.GetHeight());

    timer_.Initialize();

    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Initialize(window->GetHWND(), swapChainBuffer.GetRTVFormat());
    imguiManager->NewFrame();
}

void RenderManager::Finalize() {
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Shutdown();

    DefaultTexture::Finalize();
}

void RenderManager::Render() {

    uint32_t targetSwapChainBufferIndex = (swapChain_.GetCurrentBackBufferIndex() + 1) % SwapChain::kNumBuffers;

    commandContext_.Start(D3D12_COMMAND_LIST_TYPE_DIRECT);

    commandContext_.TransitionResource(mainColorBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.TransitionResource(mainDepthBuffer_, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    commandContext_.SetRenderTarget(mainColorBuffer_.GetRTV(), mainDepthBuffer_.GetDSV());
    commandContext_.ClearColor(mainColorBuffer_);
    commandContext_.ClearDepth(mainDepthBuffer_);
    commandContext_.SetViewportAndScissorRect(0, 0, mainColorBuffer_.GetWidth(), mainColorBuffer_.GetHeight());

    if (camera_ && sunLight_) {
        //toonRenderer_.Render(commandContext_, *camera_);
        particleRenderer_.Render(commandContext_, *camera_);
        if (raymarching_) {
            //raymarchingRenderer_.Render(commandContext_, *camera_);
            //raytracingRenderer_.Render(commandContext_, *camera_, *sunLight_);
        }
        else {
            modelRenderer.Render(commandContext_, *camera_, *sunLight_);
            raytracingRenderer_.Render(commandContext_, *camera_, *sunLight_);
        }
    }

    auto& swapChainBuffer = swapChain_.GetColorBuffer(targetSwapChainBufferIndex);
    commandContext_.TransitionResource(swapChainBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext_.SetRenderTarget(swapChainBuffer.GetRTV());
    commandContext_.ClearColor(swapChainBuffer);
    commandContext_.SetViewportAndScissorRect(0, 0, swapChainBuffer.GetWidth(), swapChainBuffer.GetHeight());

    postEffect_.Render(commandContext_, mainColorBuffer_, raytracingRenderer_.GetShadow(), raytracingRenderer_.GetReflection());
    

    spriteRenderer_.Render(commandContext_, 0.0f, 0.0f, float(swapChainBuffer.GetWidth()), float(swapChainBuffer.GetHeight()));

#ifdef _DEBUG
    ImGui::Begin("Profile");
    auto io = ImGui::GetIO();
    ImGui::Text("Framerate : %f", io.Framerate);
    ImGui::Checkbox("Raymarching", &raymarching_);
    ImGui::End();
#endif // _DEBUG

    // ImGuiを描画
    auto imguiManager = ImGuiManager::GetInstance();
    imguiManager->Render(commandContext_);

    commandContext_.TransitionResource(swapChainBuffer, D3D12_RESOURCE_STATE_PRESENT);

    // コマンドリスト完成(クローズ)
    commandContext_.Close();

    // バックバッファをフリップ
    swapChain_.Present();
    // シグナルを発行し待つ
    auto& commandQueue = graphics_->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
    commandQueue.WaitForIdle();

    commandContext_.Finish(false);

    graphics_->GetReleasedObjectTracker().FrameIncrementForRelease();

    timer_.KeepFrameRate(60);

    imguiManager->NewFrame();
}