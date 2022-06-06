//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cassert>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#include "vox.cloth/foundation/PxAllocatorCallback.h"
#include "vox.cloth/foundation/PxErrorCallback.h"
#include "vox.cloth/foundation/PxProfiler.h"
#include "vox.cloth/NvCloth/Callbacks.h"
#include "vox.cloth/NvCloth/DxContextManagerCallback.h"

#ifdef _MSC_VER
#include <Windows.h>
#endif

namespace vox::cloth {
class Allocator : public physx::PxAllocatorCallback {
public:
    Allocator() { m_enable_leak_detection_ = false; }

    void *allocate(size_t size, const char *type_name, const char *filename, int line) override {
#ifdef _MSC_VER
        void *ptr = _aligned_malloc(size, 16);
#else
        void *ptr;
        if (posix_memalign(&ptr, 16, size)) ptr = nullptr;
#endif
        if (m_enable_leak_detection_) {
            std::lock_guard<std::mutex> lock(m_allocations_map_lock_);
            m_allocations_[ptr] = Allocation(size, type_name, filename, line);
        }
        return ptr;
    }

    void deallocate(void *ptr) override {
        if (m_enable_leak_detection_ && ptr) {
            std::lock_guard<std::mutex> lock(m_allocations_map_lock_);
            auto i = m_allocations_.find(ptr);
            if (i == m_allocations_.end()) {
                printf("Tried to deallocate %p which was not allocated with this allocator callback.", ptr);
            } else {
                m_allocations_.erase(i);
            }
        }
#ifdef _MSC_VER
        _aligned_free(ptr);
#else
        free(ptr);
#endif
    }

    void StartTrackingLeaks() {
        std::lock_guard<std::mutex> lock(m_allocations_map_lock_);
        m_allocations_.clear();
        m_enable_leak_detection_ = true;
    }

    void StopTrackingLeaksAndReport() {
        std::lock_guard<std::mutex> lock(m_allocations_map_lock_);
        m_enable_leak_detection_ = false;

        size_t total_bytes = 0;
        std::stringstream message;
        message << "Memory leaks detected:\n";
        for (auto &m_allocation : m_allocations_) {
            const Allocation &alloc = m_allocation.second;
            message << "* Allocated ptr " << m_allocation.first << " of " << alloc.m_size
                    << "bytes (type=" << alloc.m_type_name << ") at " << alloc.m_file_name << ":" << alloc.m_line
                    << "\n";
            total_bytes += alloc.m_size;
        }
        if (!m_allocations_.empty()) {
            message << "=====Total of " << total_bytes << " bytes in " << m_allocations_.size()
                    << " allocations leaked=====";
            const std::string &tmp = message.str();
#ifdef _MSC_VER
            // OutputDebugString(tmp.c_str()); //Write to visual studio output so we can see it after the application
            // closes
#endif
            printf("%s\n", tmp.c_str());
        }

        m_allocations_.clear();
    }

private:
    bool m_enable_leak_detection_;

    struct Allocation {
        Allocation() = default;

        Allocation(size_t size, const char *type_name, const char *filename, int line)
            : m_size(size), m_type_name(type_name), m_file_name(filename), m_line(line) {}

        size_t m_size{};
        std::string m_type_name;
        std::string m_file_name;
        int m_line{};
    };

    std::map<void *, Allocation> m_allocations_;
    std::mutex m_allocations_map_lock_;
};

class ErrorCallback : public physx::PxErrorCallback {
public:
    ErrorCallback() = default;

    void reportError(physx::PxErrorCode::Enum code, const char *message, const char *file, int line) override;
};

class NvClothEnvironment {
    NvClothEnvironment() { SetUp(); }

    virtual ~NvClothEnvironment() { TearDown(); }

    static NvClothEnvironment *s_env_;

public:
    static void AllocateEnv() { s_env_ = new NvClothEnvironment; }

    static void FreeEnv() {
        delete s_env_;
        s_env_ = nullptr;
    }

    static void ReportEnvFreed() { s_env_ = nullptr; }  // google test will free it for us, so we just reset the value
    static NvClothEnvironment *GetEnv() { return s_env_; }

    virtual void SetUp() {
        m_allocator_ = new Allocator;
        m_allocator_->StartTrackingLeaks();
        m_foundation_allocator_ = new Allocator;
        m_foundation_allocator_->StartTrackingLeaks();
        m_error_callback_ = new ErrorCallback;
        nv::cloth::InitializeNvCloth(m_allocator_, m_error_callback_, nullptr, nullptr);
    }

    virtual void TearDown() {
        m_allocator_->StopTrackingLeaksAndReport();
        m_foundation_allocator_->StopTrackingLeaksAndReport();
        delete m_error_callback_;
        delete m_foundation_allocator_;
        delete m_allocator_;
    }

    Allocator *GetAllocator() { return m_allocator_; }

    Allocator *GetFoundationAllocator() { return m_foundation_allocator_; }

    ErrorCallback *GetErrorCallback() { return m_error_callback_; }

private:
    Allocator *m_allocator_{};
    Allocator *m_foundation_allocator_{};
    ErrorCallback *m_error_callback_{};
};

}  // namespace vox::cloth