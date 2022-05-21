//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "../tests.h"
#include "file_system.h"
#include "triangle_mesh.h"
#include "triangle_mesh_io.h"

namespace vox::tests {

TEST(FileSTL, WriteReadTriangleMeshFromSTL) {
    geometry::TriangleMesh tm_gt;
    tm_gt.vertices_ = {{0, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    tm_gt.triangles_ = {{0, 1, 2}};
    tm_gt.ComputeVertexNormals();

    const std::string tmp_stl_path = utility::filesystem::GetTempDirectoryPath() + "/tmp.stl";
    io::WriteTriangleMesh(tmp_stl_path, tm_gt);

    geometry::TriangleMesh tm_test;
    io::ReadTriangleMeshOptions opt;
    opt.print_progress = false;
    io::ReadTriangleMesh(tmp_stl_path, tm_test, opt);

    ExpectEQ(tm_gt.vertices_, tm_test.vertices_);
    ExpectEQ(tm_gt.triangles_, tm_test.triangles_);
}

}  // namespace vox::tests
