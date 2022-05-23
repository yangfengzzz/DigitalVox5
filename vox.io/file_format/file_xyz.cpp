//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <cstdio>

#include "vox.base/file_system.h"
#include "vox.base/logging.h"
#include "vox.base/progress_reporters.h"
#include "vox.io/file_format_io.h"
#include "vox.io/point_cloud_io.h"

namespace vox::io {

FileGeometry ReadFileGeometryTypeXYZ(const std::string &path) { return CONTAINS_POINTS; }

bool ReadPointCloudFromXYZ(const std::string &filename,
                           geometry::PointCloud &pointcloud,
                           const ReadPointCloudOption &params) {
    try {
        utility::filesystem::CFile file;
        if (!file.Open(filename, "r")) {
            LOGW("Read XYZ failed: unable to open file: {}", filename)
            return false;
        }
        utility::CountingProgressReporter reporter(params.update_progress);
        reporter.SetTotal(file.GetFileSize());

        pointcloud.Clear();
        int i = 0;
        double x, y, z;
        const char *line_buffer;
        while ((line_buffer = file.ReadLine())) {
            if (sscanf(line_buffer, "%lf %lf %lf", &x, &y, &z) == 3) {
                pointcloud.points_.emplace_back(x, y, z);
            }
            if (++i % 1000 == 0) {
                reporter.Update(file.CurPos());
            }
        }
        reporter.Finish();

        return true;
    } catch (const std::exception &e) {
        LOGW("Read XYZ failed with exception: {}", e.what())
        return false;
    }
}

bool WritePointCloudToXYZ(const std::string &filename,
                          const geometry::PointCloud &pointcloud,
                          const WritePointCloudOption &params) {
    try {
        utility::filesystem::CFile file;
        if (!file.Open(filename, "w")) {
            LOGW("Write XYZ failed: unable to open file: {}", filename)
            return false;
        }
        utility::CountingProgressReporter reporter(params.update_progress);
        reporter.SetTotal(pointcloud.points_.size());

        for (size_t i = 0; i < pointcloud.points_.size(); i++) {
            const Eigen::Vector3d &point = pointcloud.points_[i];
            if (fprintf(file.GetFILE(), "%.10f %.10f %.10f\n", point(0), point(1), point(2)) < 0) {
                LOGW("Write XYZ failed: unable to write file: {}", filename)
                return false;  // error happened during writing.
            }
            if (i % 1000 == 0) {
                reporter.Update(i);
            }
        }
        reporter.Finish();
        return true;
    } catch (const std::exception &e) {
        LOGW("Write XYZ failed with exception: {}", e.what())
        return false;
    }
}

}  // namespace vox::io
