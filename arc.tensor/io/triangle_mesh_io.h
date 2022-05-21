//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

#include "../../arc.io/triangle_mesh_io.h"
#include "geometry/triangle_mesh.h"

namespace vox {
namespace t {
namespace io {

/// Factory function to create a mesh from a file (TriangleMeshFactory.cpp)
/// Return an empty mesh if fail to read the file.
std::shared_ptr<geometry::TriangleMesh> CreateMeshFromFile(const std::string &filename, bool print_progress = false);

/// The general entrance for reading a TriangleMesh from a file
/// The function calls read functions based on the extension name of filename.
/// \return return true if the read function is successful, false otherwise.
bool ReadTriangleMesh(const std::string &filename,
                      geometry::TriangleMesh &mesh,
                      arc::io::ReadTriangleMeshOptions params = {});

/// The general entrance for writing a TriangleMesh to a file
/// The function calls write functions based on the extension name of filename.
/// If the write function supports binary encoding and compression, the later
/// two parameters will be used. Otherwise they will be ignored.
/// At current only .obj format supports uv coordinates (triangle_uvs) and
/// textures.
/// \return return true if the write function is successful, false otherwise.
bool WriteTriangleMesh(const std::string &filename,
                       const geometry::TriangleMesh &mesh,
                       bool write_ascii = false,
                       bool compressed = false,
                       bool write_vertex_normals = true,
                       bool write_vertex_colors = true,
                       bool write_triangle_uvs = true,
                       bool print_progress = false);

}  // namespace io
}  // namespace t
}  // namespace vox
