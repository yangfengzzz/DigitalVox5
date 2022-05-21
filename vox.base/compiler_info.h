//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.
#pragma once

#include <memory>
#include <string>

namespace vox::utility {

/// \brief Compiler information.
class CompilerInfo {
    // This does not need to be a class. It is a class just for the sake of
    // consistency with CPUInfo.
public:
    static CompilerInfo& GetInstance();

    ~CompilerInfo() = default;
    CompilerInfo(const CompilerInfo&) = delete;
    void operator=(const CompilerInfo&) = delete;

    static std::string CXXStandard();

    static std::string CXXCompilerId();
    static std::string CXXCompilerVersion();

    static std::string CUDACompilerId();
    static std::string CUDACompilerVersion();

    void Print() const;

private:
    CompilerInfo();
};

}  // namespace vox::utility
