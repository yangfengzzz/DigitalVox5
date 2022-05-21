//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <cstdio>

#include "../file_format_io.h"
#include "../point_cloud_io.h"
#include "file_system.h"
#include "logging.h"
#include "progress_reporters.h"

namespace vox::io {

FileGeometry ReadFileGeometryTypeXYZN(const std::string &path) { return CONTAINS_POINTS; }

bool ReadPointCloudFromXYZN(const std::string &filename,
                            geometry::PointCloud &pointcloud,
                            const ReadPointCloudOption &params) {
    try {
        utility::filesystem::CFile file;
        if (!file.Open(filename, "r")) {
            LOGW("Read XYZN failed: unable to open file: {}", filename)
            return false;
        }
        utility::CountingProgressReporter reporter(params.update_progress);
        reporter.SetTotal(file.GetFileSize());

        pointcloud.Clear();
        int i = 0;
        double x, y, z, nx, ny, nz;
        const char *line_buffer;
        while ((line_buffer = file.ReadLine())) {
            if (sscanf(line_buffer, "%lf %lf %lf %lf %lf %lf", &x, &y, &z, &nx, &ny, &nz) == 6) {
                pointcloud.points_.emplace_back(x, y, z);
                pointcloud.normals_.emplace_back(nx, ny, nz);
            }
            if (++i % 1000 == 0) {
                reporter.Update(file.CurPos());
            }
        }
        reporter.Finish();

        return true;
    } catch (const std::exception &e) {
        LOGW("Read XYZN failed with exception: {}", e.what())
        return false;
    }
}

bool WritePointCloudToXYZN(const std::string &filename,
                           const geometry::PointCloud &pointcloud,
                           const WritePointCloudOption &params) {
    if (!pointcloud.HasNormals()) {
        return false;
    }

    try {
        utility::filesystem::CFile file;
        if (!file.Open(filename, "w")) {
            LOGW("Write XYZN failed: unable to open file: {}", filename)
            return false;
        }
        utility::CountingProgressReporter reporter(params.update_progress);
        reporter.SetTotal(pointcloud.points_.size());

        for (size_t i = 0; i < pointcloud.points_.size(); i++) {
            const Eigen::Vector3d &point = pointcloud.points_[i];
            const Eigen::Vector3d &normal = pointcloud.normals_[i];
            if (fprintf(file.GetFILE(), "%.10f %.10f %.10f %.10f %.10f %.10f\n", point(0), point(1), point(2),
                        normal(0), normal(1), normal(2)) < 0) {
                LOGW("Write XYZN failed: unable to write file: {}", filename)
                return false;  // error happened during writing.
            }
            if (i % 1000 == 0) {
                reporter.Update(i);
            }
        }
        reporter.Finish();
        return true;
    } catch (const std::exception &e) {
        LOGW("Write XYZN failed with exception: {}", e.what())
        return false;
    }
}

}  // namespace vox::io
