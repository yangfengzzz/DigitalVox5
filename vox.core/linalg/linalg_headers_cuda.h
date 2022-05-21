//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

// This file contains headers for BLAS/LAPACK implementations for CUDA.
//
// For developers, please make sure that this file is not ultimately included in
// Open3D.h.

#pragma once

#ifdef BUILD_CUDA_MODULE
#include <cublas_v2.h>
#include <cusolverDn.h>
#include <cusolver_common.h>
#endif
