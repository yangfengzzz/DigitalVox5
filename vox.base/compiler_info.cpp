// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018-2021 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#include "compiler_info.h"

#include <memory>

#include "logging.h"

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
