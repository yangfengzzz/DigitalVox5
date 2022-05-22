//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.base/parallel.h"

#ifdef _OPENMP
#include <omp.h>
#endif

#include <string>

#include "vox.base/cpu_info.h"

namespace vox::utility {

static std::string GetEnvVar(const std::string& name) {
    if (const char* value = std::getenv(name.c_str())) {
        return {value};
    } else {
        return "";
    }
}

int EstimateMaxThreads() {
#ifdef _OPENMP
    if (!GetEnvVar("OMP_NUM_THREADS").empty() || !GetEnvVar("OMP_DYNAMIC").empty()) {
        // See the full list of OpenMP environment variables at:
        // https://www.openmp.org/spec-html/5.0/openmpch6.html
        return omp_get_max_threads();
    } else {
        // Returns the number of physical cores.
        return utility::CPUInfo::GetInstance().NumCores();
    }
#else
    (void)&GetEnvVar;  // Avoids compiler warning.
    return 1;
#endif
}

bool InParallel() {
    // TODO: when we add TBB/Parallel STL support to ParallelFor, update this.
#ifdef _OPENMP
    return omp_in_parallel();
#else
    return false;
#endif
}

}  // namespace vox::utility
