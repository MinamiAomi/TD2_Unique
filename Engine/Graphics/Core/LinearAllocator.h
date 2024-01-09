#pragma once

#include <memory>
#include <queue>
#include <mutex>

#include "UploadBuffer.h"

class LinearAllocator {
public:
    static const size_t kPageSize = 2000000;

    struct Allocation {
        void* cpu;
        const D3D12_GPU_VIRTUAL_ADDRESS gpu;
    };

    static void Finalize();
    
    Allocation Allocate(size_t sizeInByte, size_t alignment = 256);
    void Reset(D3D12_COMMAND_LIST_TYPE type, UINT64 fenceValue);

private:
    class Page {
    public:
        Page();

        bool HasSpace(size_t sizeInByte, size_t alignment);
        Allocation Allocate(size_t sizeInByte, size_t alignment);
        void Reset();

    private:
        UploadBuffer buffer_;
        size_t offset_;
    };

    using PagePtr = std::shared_ptr<Page>;

    class PagePool {
    public:
        PagePtr Allocate();
        void Discard(D3D12_COMMAND_LIST_TYPE type, UINT64 fenceValue, const std::vector<PagePtr>& pages);
        void Clear();

        size_t GetSize() const { return pagePool_.size(); }

    private:
        std::queue<std::pair<UINT64, PagePtr>>& GetReadyPages(D3D12_COMMAND_LIST_TYPE type) {
            switch (type) {
            case D3D12_COMMAND_LIST_TYPE_COMPUTE:
                return computeReadyPages_;
            case D3D12_COMMAND_LIST_TYPE_COPY:
                return copyReadyPages_;
            }
            return directReadyPages_;
        }

        PagePtr TryAllocate(D3D12_COMMAND_LIST_TYPE type);

        std::vector<PagePtr> pagePool_;
        std::queue<std::pair<UINT64, PagePtr>> directReadyPages_;
        std::queue<std::pair<UINT64, PagePtr>> computeReadyPages_;
        std::queue<std::pair<UINT64, PagePtr>> copyReadyPages_;
        std::mutex mutex_;
    };

    static PagePool pagePool_;

    std::vector<PagePtr> usedPages_;
    PagePtr currentPage_;
};