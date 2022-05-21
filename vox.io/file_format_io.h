//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

namespace vox::io {

enum FileGeometry {
    CONTENTS_UNKNOWN = 0,
    CONTAINS_POINTS = (1 << 0),
    CONTAINS_LINES = (1 << 1),
    CONTAINS_TRIANGLES = (1 << 2),
};

/// Returns the kind of geometry that the file contains. This is a quick
/// function designed to query the file in order to determine whether to
/// call ReadTriangleMesh(), ReadLineSet(), or ReadPointCloud()
FileGeometry ReadFileGeometryType(const std::string& path);

FileGeometry ReadFileGeometryTypeGLTF(const std::string& path);
FileGeometry ReadFileGeometryTypeOBJ(const std::string& path);
FileGeometry ReadFileGeometryTypeFBX(const std::string& path);
FileGeometry ReadFileGeometryTypeOFF(const std::string& path);
FileGeometry ReadFileGeometryTypePCD(const std::string& path);
FileGeometry ReadFileGeometryTypePLY(const std::string& path);
FileGeometry ReadFileGeometryTypePTS(const std::string& path);
FileGeometry ReadFileGeometryTypeSTL(const std::string& path);
FileGeometry ReadFileGeometryTypeXYZ(const std::string& path);
FileGeometry ReadFileGeometryTypeXYZN(const std::string& path);
FileGeometry ReadFileGeometryTypeXYZRGB(const std::string& path);

}  // namespace vox::io
