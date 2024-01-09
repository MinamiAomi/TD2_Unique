#include "LinearAllocator.h"

#include <assert.h>

#include "Graphics.h"
#include "Helper.h"

LinearAllocator::PagePool LinearAllocator::pagePool_;

void LinearAllocator::Finalize() {
    pagePool_.Clear();
}

LinearAllocator::Allocation LinearAllocator::Allocate(size_t sizeInByte, size_t alignment) {
    assert(sizeInByte <= kPageSize);

    // 現在のページがない、または、ページに空きがない場合
    if (!currentPage_ || !currentPage_->HasSpace(sizeInByte, alignment)) {
        if (currentPage_) {
            usedPages_.emplace_back(currentPage_);
        }
        // 新しいページを要求
        currentPage_ = pagePool_.Allocate();
    }
    // 割り当て
    return currentPage_->Allocate(sizeInByte, alignment);
}

void LinearAllocator::Reset(D3D12_COMMAND_LIST_TYPE type, UINT64 fenceValue) {
    if (currentPage_) {
        usedPages_.emplace_back(currentPage_);
    }
    if (!usedPages_.empty()) {
        pagePool_.Discard(type, fenceValue, usedPages_);
        usedPages_.clear();
    }
}

LinearAllocator::Page::Page() :
    offset_(0) {
    buffer_.Create(L"LinearAllocator Page", LinearAllocator::kPageSize);
}

bool LinearAllocator::Page::HasSpace(size_t sizeInByte, size_t alignment) {
    size_t alignedSize = Helper::AlignUp(sizeInByte, alignment);
    size_t alignedOffset = Helper::AlignUp(offset_, alignment);

    return alignedOffset + alignedSize <= buffer_.GetBufferSize();
}

LinearAllocator::Allocation LinearAllocator::Page::Allocate(size_t sizeInByte, size_t alignment) {
    size_t alignedSize = Helper::AlignUp(sizeInByte, alignment);
    offset_ = Helper::AlignUp(offset_, alignment);

    Allocation allocation{
        .cpu = static_cast<uint8_t*>(buffer_.GetCPUDataBegin()) + offset_,
        .gpu = buffer_.GetGPUVirtualAddress() + offset_
    };
    offset_ += alignedSize;
    return allocation;
}

void LinearAllocator::Page::Reset() {
    offset_ = 0;
}

LinearAllocator::PagePtr LinearAllocator::PagePool::Allocate() {
    std::lock_guard<std::mutex> lock(mutex_);

    PagePtr page;

    page = TryAllocate(D3D12_COMMAND_LIST_TYPE_DIRECT);
    if (page) { return page; }
    page = TryAllocate(D3D12_COMMAND_LIST_TYPE_COMPUTE);
    if (page) { return page; }
    page = TryAllocate(D3D12_COMMAND_LIST_TYPE_COPY);
    if (page) { return page; }

    page = std::make_shared<Page>();
    pagePool_.emplace_back(page);

    return page;
}

void LinearAllocator::PagePool::Discard(D3D12_COMMAND_LIST_TYPE type, UINT64 fenceValue, const std::vector<PagePtr>& pages) {
    auto& readyPages = GetReadyPages(type);
    for (auto& page : pages) {
        readyPages.push(std::make_pair(fenceValue, page));
    }
}

void LinearAllocator::PagePool::Clear() {
    pagePool_.clear();
    std::queue<std::pair<UINT64, PagePtr>>().swap(directReadyPages_);
    std::queue<std::pair<UINT64, PagePtr>>().swap(computeReadyPages_);
    std::queue<std::pair<UINT64, PagePtr>>().swap(copyReadyPages_);
}

LinearAllocator::PagePtr LinearAllocator::PagePool::TryAllocate(D3D12_COMMAND_LIST_TYPE type) {
    auto& readyPages = GetReadyPages(type);

    if (!readyPages.empty()) {
        const auto& [fenceValue, readyPage] = readyPages.front();
        auto& queue = Graphics::GetInstance()->GetCommandQueue(type);

        if (fenceValue <= queue.GetLastCompletedFenceValue()) {
            readyPage->Reset();
            readyPages.pop();
            return readyPage;
        }
    }

    return PagePtr();
}
