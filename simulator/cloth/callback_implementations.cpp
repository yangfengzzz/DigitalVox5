//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "simulator/cloth/callback_implementations.h"

#include <iostream>

namespace vox::cloth {
NvClothEnvironment *NvClothEnvironment::s_env_ = nullptr;

void ErrorCallback::reportError(physx::PxErrorCode::Enum code, const char *message, const char *file, int line) {
    const char *code_name = "???";
    switch (code) {
#define CASE(x)                       \
    case physx::PxErrorCode::Enum::x: \
        code_name = #x;                \
        break;
        CASE(eNO_ERROR)
        CASE(eDEBUG_INFO)
        CASE(eDEBUG_WARNING)
        CASE(eINVALID_PARAMETER)
        CASE(eINVALID_OPERATION)
        CASE(eOUT_OF_MEMORY)
        CASE(eINTERNAL_ERROR)
        CASE(eABORT)
        CASE(ePERF_WARNING)
        default:;
#undef CASE
    }

    std::cout << "Log " << code_name << " from file:" << file << ":" << line << "\n MSG:" << message << std::endl;
}

}  // namespace vox
