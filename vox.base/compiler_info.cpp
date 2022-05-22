//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.base/compiler_info.h"

#include <memory>

#include "vox.base/logging.h"

namespace vox::utility {

CompilerInfo::CompilerInfo() = default;

CompilerInfo& CompilerInfo::GetInstance() {
    static CompilerInfo instance;
    return instance;
}

std::string CompilerInfo::CXXStandard() { return {OPEN3D_CXX_STANDARD}; }

std::string CompilerInfo::CXXCompilerId() { return {OPEN3D_CXX_COMPILER_ID}; }

std::string CompilerInfo::CXXCompilerVersion() { return {OPEN3D_CXX_COMPILER_VERSION}; }

std::string CompilerInfo::CUDACompilerId() { return {OPEN3D_CUDA_COMPILER_ID}; }

std::string CompilerInfo::CUDACompilerVersion() { return {OPEN3D_CUDA_COMPILER_VERSION}; }

void CompilerInfo::Print() const {
#ifdef BUILD_CUDA_MODULE
    LOGI("CompilerInfo: C++ {}, {} {}, {} {}.", CXXStandard(), CXXCompilerId(), CXXCompilerVersion(), CUDACompilerId(),
         CUDACompilerVersion())
#else
    LOGI("CompilerInfo: C++ {}, {} {}, CUDA disabled.", CXXStandard(), CXXCompilerId(), CXXCompilerVersion())
#endif
}

}  // namespace vox::utility
