//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.
#pragma once

#include <memory>

namespace vox::utility {

/// \brief CPU information.
class CPUInfo {
public:
    static CPUInfo& GetInstance();

    ~CPUInfo() = default;
    CPUInfo(const CPUInfo&) = delete;
    void operator=(const CPUInfo&) = delete;

    /// Returns the number of physical CPU cores.
    /// This is similar to boost::thread::physical_concurrency().
    [[nodiscard]] int NumCores() const;

    /// Returns the number of logical CPU cores.
    /// This returns the same result as std::thread::hardware_concurrency() or
    /// boost::thread::hardware_concurrency().
    [[nodiscard]] int NumThreads() const;

    /// Prints CPUInfo in the console.
    void Print() const;

private:
    CPUInfo();
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vox::utility
