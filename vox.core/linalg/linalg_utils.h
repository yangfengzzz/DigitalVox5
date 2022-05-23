//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <string>

#include "vox.base/logging.h"
#include "vox.core/dtype.h"
#include "vox.core/linalg/linalg_headers_cpu.h"
#include "vox.core/linalg/linalg_headers_cuda.h"
#include "vox.core/memory_manager.h"

namespace vox {
namespace core {

#define DISPATCH_LINALG_DTYPE_TO_TEMPLATE(DTYPE, ...) \
    [&] {                                             \
        if (DTYPE == vox::core::Float32) {            \
            using scalar_t = float;                   \
            return __VA_ARGS__();                     \
        } else if (DTYPE == vox::core::Float64) {     \
            using scalar_t = double;                  \
            return __VA_ARGS__();                     \
        } else {                                      \
            LOGE("Unsupported data type.");           \
        }                                             \
    }()

inline void OPEN3D_LAPACK_CHECK(OPEN3D_CPU_LINALG_INT info, const std::string& msg) {
    if (info < 0) {
        LOGE("{}: {}-th parameter is invalid.", msg, -info);
    } else if (info > 0) {
        LOGE("{}: singular condition detected.", msg);
    }
}

#ifdef BUILD_CUDA_MODULE
inline void OPEN3D_CUBLAS_CHECK(cublasStatus_t status, const std::string& msg) {
    if (CUBLAS_STATUS_SUCCESS != status) {
        LOGE("{}", msg);
    }
}

inline void OPEN3D_CUSOLVER_CHECK(cusolverStatus_t status, const std::string& msg) {
    if (CUSOLVER_STATUS_SUCCESS != status) {
        LOGE("{}", msg);
    }
}

inline void OPEN3D_CUSOLVER_CHECK_WITH_DINFO(cusolverStatus_t status,
                                             const std::string& msg,
                                             int* dinfo,
                                             const Device& device) {
    int hinfo;
    MemoryManager::MemcpyToHost(&hinfo, dinfo, device, sizeof(int));
    if (status != CUSOLVER_STATUS_SUCCESS || hinfo != 0) {
        if (hinfo < 0) {
            LOGE("{}: {}-th parameter is invalid.", msg, -hinfo);
        } else if (hinfo > 0) {
            LOGE("{}: singular condition detected.", msg);
        } else {
            LOGE("{}: status error code = {}.", msg, status);
        }
    }
}

class CuSolverContext {
public:
    static std::shared_ptr<CuSolverContext> GetInstance();
    CuSolverContext();
    ~CuSolverContext();

    cusolverDnHandle_t& GetHandle() { return handle_; }

private:
    cusolverDnHandle_t handle_;

    static std::shared_ptr<CuSolverContext> instance_;
};

class CuBLASContext {
public:
    static std::shared_ptr<CuBLASContext> GetInstance();

    CuBLASContext();
    ~CuBLASContext();

    cublasHandle_t& GetHandle() { return handle_; }

private:
    cublasHandle_t handle_;

    static std::shared_ptr<CuBLASContext> instance_;
};
#endif
}  // namespace core
}  // namespace vox
