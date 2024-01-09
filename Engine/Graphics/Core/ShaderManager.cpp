#include "ShaderManager.h"

#include <cassert>
#include <format>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "Helper.h"

#pragma comment(lib,"dxcompiler.lib")

using namespace Microsoft::WRL;

namespace {

    std::wstring profiles[ShaderManager::kNumTypes] = {
        L"vs_6_5",
        L"ps_6_5",
        L"gs_6_5",
        L"hs_6_5",
        L"ds_6_5",
        L"cs_6_5"
    };

    std::wstring GetProfile(ShaderType type, int majorVersion, int minorVersion) {
        const wchar_t* kProfileTypes[] = {
            L"vs_",
            L"hs_",
            L"ds_",
            L"gs_",
            L"ps_",
            L"cs_",
            L"lib_",
        };
        std::wostringstream stream;
        stream << kProfileTypes[static_cast<size_t>(type)] << majorVersion << L"_" << minorVersion;
        return stream.str();
    }

}

ShaderManager* ShaderManager::GetInstance() {
    static ShaderManager instance;
    return &instance;
}

void ShaderManager::Initialize() {
    ASSERT_IF_FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils_.GetAddressOf())));
    ASSERT_IF_FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(compiler_.GetAddressOf())));
    ASSERT_IF_FAILED(utils_->CreateDefaultIncludeHandler(includeHandler_.GetAddressOf()));
    directory_ = std::filesystem::current_path();
}

Microsoft::WRL::ComPtr<IDxcBlob> ShaderManager::Compile(const std::filesystem::path& path, Type type) {
    auto fullpath = directory_ / path;
    return Compile(fullpath, profiles[type]);
}

Microsoft::WRL::ComPtr<IDxcBlob> ShaderManager::Compile(const std::filesystem::path& path, ShaderType type, int majorVersion, int minorVersion) {
    std::wstring profile = GetProfile(type, majorVersion, minorVersion);
    std::wstring fullpath = directory_ / path;
    return Compile(fullpath, profile);
}

Microsoft::WRL::ComPtr<IDxcBlob> ShaderManager::Compile(const std::wstring& path, const std::wstring& profile) {
    OutputDebugStringW(std::format(L"Begin CompileShader, path:{}, profile:{}\n", path, profile).c_str());

    //MessageBoxW(nullptr, path.wstring().c_str(), L"Cap", S_OK);

    ComPtr<IDxcBlobEncoding> shaderSource;
    ASSERT_IF_FAILED(utils_->LoadFile(path.c_str(), nullptr, shaderSource.GetAddressOf()));

    DxcBuffer shader_source_buffer{};
    shader_source_buffer.Ptr = shaderSource->GetBufferPointer();
    shader_source_buffer.Size = shaderSource->GetBufferSize();
    shader_source_buffer.Encoding = DXC_CP_UTF8;

    LPCWSTR arguments[] = {
        path.c_str(),
        L"-E", L"main",
        L"-T", profile.c_str(),
        L"-Zi", L"-Qembed_debug",
        L"-Od",
        L"-Zpr"
    };

    ComPtr<IDxcResult> shaderResult;
    ASSERT_IF_FAILED(compiler_->Compile(
        &shader_source_buffer,
        arguments,
        _countof(arguments),
        includeHandler_.Get(),
        IID_PPV_ARGS(shaderResult.GetAddressOf())));

    ComPtr<IDxcBlobUtf8> shaderError = nullptr;
    ASSERT_IF_FAILED(shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(shaderError.GetAddressOf()), nullptr));

    if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
        std::string errorMsg = shaderError->GetStringPointer();
        MessageBoxA(nullptr, errorMsg.c_str(), "Failed shader compile", S_OK);
        OutputDebugStringA(errorMsg.c_str());
        assert(false);
    }

    ComPtr<IDxcBlob> blob;
    ASSERT_IF_FAILED(shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(blob.GetAddressOf()), nullptr));

    OutputDebugStringW(std::format(L"Compile Succeeded, path:{}, profile:{}\n", path, profile).c_str());
    return blob;
}
