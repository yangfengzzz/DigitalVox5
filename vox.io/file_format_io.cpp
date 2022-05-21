//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "file_format_io.h"

#include <map>

#include "file_system.h"

namespace vox::io {

static std::map<std::string, FileGeometry (*)(const std::string&)> gExt2Func = {
        {"glb", ReadFileGeometryTypeGLTF}, {"gltf", ReadFileGeometryTypeGLTF}, {"obj", ReadFileGeometryTypeOBJ},
        {"fbx", ReadFileGeometryTypeFBX},  {"off", ReadFileGeometryTypeOFF},   {"pcd", ReadFileGeometryTypePCD},
        {"ply", ReadFileGeometryTypePLY},  {"pts", ReadFileGeometryTypePTS},   {"stl", ReadFileGeometryTypeSTL},
        {"xyz", ReadFileGeometryTypeXYZ},  {"xyzn", ReadFileGeometryTypeXYZN}, {"xyzrgb", ReadFileGeometryTypeXYZRGB},
};

FileGeometry ReadFileGeometryType(const std::string& path) {
    auto ext = utility::filesystem::GetFileExtensionInLowerCase(path);
    auto it = gExt2Func.find(ext);
    if (it != gExt2Func.end()) {
        return it->second(path);
    } else {
        return CONTENTS_UNKNOWN;
    }
}

}  // namespace vox::io
