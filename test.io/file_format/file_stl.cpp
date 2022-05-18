// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018-2021 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#include "../tests.h"
#include "file_system.h"
#include "triangle_mesh.h"
#include "triangle_mesh_io.h"

namespace arc::tests {

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

}  // namespace arc::tests
