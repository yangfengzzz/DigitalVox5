//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <fstream>
#include <vector>

#include "../file_format_io.h"
#include "../triangle_mesh_io.h"
#include "logging.h"
#include "progress_bar.h"

namespace vox::io {

FileGeometry ReadFileGeometryTypeSTL(const std::string &path) {
    return FileGeometry(CONTAINS_TRIANGLES | CONTAINS_POINTS);
}

bool WriteTriangleMeshToSTL(const std::string &filename,
                            const geometry::TriangleMesh &mesh,
                            bool write_ascii /* = false*/,
                            bool compressed /* = false*/,
                            bool write_vertex_normals /* = true*/,
                            bool write_vertex_colors /* = true*/,
                            bool write_triangle_uvs /* = true*/,
                            bool print_progress) {
    if (write_triangle_uvs && mesh.HasTriangleUvs()) {
        LOGW("This file format does not support writing textures and uv "
             "coordinates. Consider using .obj")
    }
    if (write_ascii) {
        LOGE("Writing ascii STL file is not supported yet.")
    }

    std::ofstream myFile(filename.c_str(), std::ios::out | std::ios::binary);

    if (!myFile) {
        LOGW("Write STL failed: unable to open file.")
        return false;
    }

    if (!mesh.HasTriangleNormals()) {
        LOGW("Write STL failed: compute normals first.")
        return false;
    }

    size_t num_of_triangles = mesh.triangles_.size();
    if (num_of_triangles == 0) {
        LOGW("Write STL failed: empty file.")
        return false;
    }
    char header[80] = "Created by Open3D";
    myFile.write(header, 80);
    myFile.write((char *)(&num_of_triangles), 4);

    utility::ProgressBar progress_bar(num_of_triangles, "Writing STL: ", print_progress);
    for (size_t i = 0; i < num_of_triangles; i++) {
        Eigen::Vector3f float_vector3f = mesh.triangle_normals_[i].cast<float>();
        myFile.write(reinterpret_cast<const char *>(float_vector3f.data()), 12);
        for (int j = 0; j < 3; j++) {
            Eigen::Vector3f float_vector3f = mesh.vertices_[mesh.triangles_[i][j]].cast<float>();
            myFile.write(reinterpret_cast<const char *>(float_vector3f.data()), 12);
        }
        char blank[2] = {0, 0};
        myFile.write(blank, 2);
        ++progress_bar;
    }
    return true;
}

}  // namespace vox::io
