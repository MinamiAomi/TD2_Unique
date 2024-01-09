#include "Graphics.h"

#include <dxgi1_6.h>

#include <cassert>
#include <format>

#include "Helper.h"
#include "SamplerManager.h"
#include "TextureLoader.h"
#include "LinearAllocator.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")

using namespace Microsoft::WRL;

#define ENABLED_DEBUG_LAYER 1
#define ENABLED_GPU_BASED_DEBUGGER 1

#ifdef _DEBUG

#include <dxgidebug.h>

namespace {
    struct LeakChecker {
        ~LeakChecker() {
            ComPtr<IDXGIDebug1> debug;
            if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug.GetAddressOf())))) {
                debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
                debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
                debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
            }
        }
    } leakChecker;

}
#endif // _DEBUG

Graphics* Graphics::GetInstance() {
    static Graphics instance;
    return &instance;
}

void Graphics::Initialize() {
    CreateDevice();

    D3D12_FEATURE_DATA_SHADER_MODEL featureShaderModel{};
    if (SUCCEEDED(device_->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &featureShaderModel, sizeof(featureShaderModel)))) {
        if (featureShaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_6) {
            // HLSL 6.6に対応してない
            assert(false);
        }

    }

    D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5{};
    if (SUCCEEDED(device_->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5)))) {
        if (options5.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED) {
            ASSERT_IF_FAILED(device_.As(&dxrDevice_));
            OutputDebugStringA("DXR supported!!\n");
        }
    }

    directCommandSet_.queue.Create();
    computeCommandSet_.queue.Create();
    copyCommandSet_.queue.Create();

    uint32_t numDescriptorsTable[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
    numDescriptorsTable[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] = kNumRTVs;
    numDescriptorsTable[D3D12_DESCRIPTOR_HEAP_TYPE_DSV] = kNumDSVs;
    numDescriptorsTable[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = kNumSRVs;
    numDescriptorsTable[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER] = kNumSamplers;

    for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
        descriptorHeaps_[i] = std::make_shared<DescriptorHeap>();
        descriptorHeaps_[i]->Create(D3D12_DESCRIPTOR_HEAP_TYPE(i), numDescriptorsTable[i]);
    }

    SamplerManager::Initialize();
    CreateDynamicResourcesRootSignature();
}

void Graphics::Finalize() {
    directCommandSet_.queue.WaitForIdle();
    computeCommandSet_.queue.WaitForIdle();
    copyCommandSet_.queue.WaitForIdle();
    TextureLoader::ReleaseAll();
    LinearAllocator::Finalize();
    releasedObjectTracker_.AllRelease();
}

DescriptorHandle Graphics::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type) {
    return descriptorHeaps_[type]->Allocate();
}

Graphics::Graphics() :
    directCommandSet_(D3D12_COMMAND_LIST_TYPE_DIRECT),
    computeCommandSet_(D3D12_COMMAND_LIST_TYPE_COMPUTE),
    copyCommandSet_(D3D12_COMMAND_LIST_TYPE_COPY) {
}

void Graphics::CreateDevice() {
#ifdef _DEBUG
#if ENABLED_DEBUG_LAYER || ENABLED_GPU_BASED_DEBUGGER
    ComPtr<ID3D12Debug1> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf())))) {
#if ENABLED_DEBUG_LAYER 
        debugController->EnableDebugLayer();
#endif
#if ENABLED_GPU_BASED_DEBUGGER
        debugController->SetEnableGPUBasedValidation(TRUE);
#endif
    }
#endif
#endif // _DEBUG

    ComPtr<IDXGIFactory7> factory;
    ASSERT_IF_FAILED(CreateDXGIFactory(IID_PPV_ARGS(factory.GetAddressOf())));

    ComPtr<IDXGIAdapter4> adapter;
    for (uint32_t i = 0;
        factory->EnumAdapterByGpuPreference(
            i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
            IID_PPV_ARGS(adapter.GetAddressOf())) != DXGI_ERROR_NOT_FOUND;
        ++i) {
        // アダプター情報を取得
        DXGI_ADAPTER_DESC3 adapterDesc{};
        ASSERT_IF_FAILED(adapter->GetDesc3(&adapterDesc));
        // ソフトウェアアダプタでなければ採用
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            // 採用したアダプタ情報を出力
            OutputDebugStringW(std::format(L"Use Adapter:{}\n", adapterDesc.Description).c_str());
            break;
        }
        adapter.Reset(); // ソフトウェアアダプタは見なかったことにする
    }
    assert(adapter);

    // 機能レベルとログ出力用の文字列
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
    };
    const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };
    // 高い順に生成できるか試していく
    for (size_t i = 0; i < _countof(featureLevels); ++i) {
        // 採用したアダプターデバイスを生成
        // 指定した機能レベルでデバイスが生成できたかを確認
        if (SUCCEEDED(D3D12CreateDevice(
            adapter.Get(), featureLevels[i], IID_PPV_ARGS(device_.GetAddressOf())))) {
            // 生成できたのでログ出力を行ってループを抜ける
            OutputDebugStringA(std::format("FeatureLevel : {}\n", featureLevelStrings[i]).c_str());
            break;
        }
    }
    assert(device_);

#ifdef _DEBUG
    // デバッグ時のみ
    ComPtr<ID3D12InfoQueue> infoQueue;
    if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(infoQueue.GetAddressOf())))) {
        // やばいエラーの時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        // エラーの時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        // 警告時に止まる
        //infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        // 抑制するメッセージのID
        D3D12_MESSAGE_ID denyIds[] = {
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
        };
        // 抑制するレベル
        D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;
        // 指定したメッセージの表示を抑制する
        infoQueue->PushStorageFilter(&filter);
    }
#endif
}

void Graphics::CreateDynamicResourcesRootSignature() {
    D3D12_ROOT_SIGNATURE_DESC dynamicResourcesRootSignatureDesc{};
    dynamicResourcesRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
    dynamicResourcesRootSignatureDesc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    dynamicResourcesRootSignatureDesc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
    dynamicResourcesRootSignatureDesc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;

    CD3DX12_ROOT_PARAMETER rootParameters[1]{};
    rootParameters[0].InitAsConstantBufferView(0);
    dynamicResourcesRootSignatureDesc.pParameters = rootParameters;
    dynamicResourcesRootSignatureDesc.NumParameters = _countof(rootParameters);
    dynamicResourcesRootSignature_.Create(L"RootSignature DynamicResources", dynamicResourcesRootSignatureDesc);
}
