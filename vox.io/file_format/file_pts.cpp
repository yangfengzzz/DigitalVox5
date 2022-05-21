//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <cstdio>

#include "../file_format_io.h"
#include "../point_cloud_io.h"
#include "file_system.h"
#include "helper.h"
#include "logging.h"
#include "progress_reporters.h"

namespace vox::io {

FileGeometry ReadFileGeometryTypePTS(const std::string &path) { return CONTAINS_POINTS; }

bool ReadPointCloudFromPTS(const std::string &filename,
                           geometry::PointCloud &pointcloud,
                           const ReadPointCloudOption &params) {
    try {
        utility::filesystem::CFile file;
        if (!file.Open(filename, "r")) {
            LOGW("Read PTS failed: unable to open file: {}", filename)
            return false;
        }
        size_t num_of_pts = 0;
        size_t num_of_fields = 0;
        const char *line_buffer;
        if ((line_buffer = file.ReadLine())) {
            sscanf(line_buffer, "%zu", &num_of_pts);
        }
        if (num_of_pts <= 0) {
            LOGW("Read PTS failed: unable to read header.")
            return false;
        }
        utility::CountingProgressReporter reporter(params.update_progress);
        reporter.SetTotal(num_of_pts);

        pointcloud.Clear();

        // Store data start position.
        int64_t start_pos = ftell(file.GetFILE());

        if ((line_buffer = file.ReadLine())) {
            num_of_fields = utility::SplitString(line_buffer, " ").size();

            if (num_of_fields == 7 || num_of_fields == 4) {
                LOGW("Read PTS: only points and colors attributes are "
                     "supported.")
            }

            // X Y Z I R G B or X Y Z R G B.
            if (num_of_fields == 7 || num_of_fields == 6) {
                pointcloud.points_.resize(num_of_pts);
                pointcloud.colors_.resize(num_of_pts);
            }
            // X Y Z I or X Y Z.
            else if (num_of_fields == 4 || num_of_fields == 3) {
                pointcloud.points_.resize(num_of_pts);
            } else {
                LOGW("Read PTS failed: unknown pts format: {}", line_buffer)
                return false;
            }
        }

        // Go to data start position.
        fseek(file.GetFILE(), start_pos, 0);

        size_t idx = 0;
        while (idx < num_of_pts && (line_buffer = file.ReadLine())) {
            double x, y, z, i;
            int r, g, b;
            // X Y Z I R G B
            if (num_of_fields == 7 &&
                sscanf(line_buffer, "%lf %lf %lf %lf %d %d %d", &x, &y, &z, &i, &r, &g, &b) == 7) {
                pointcloud.points_[idx] = Eigen::Vector3d(x, y, z);
                pointcloud.colors_[idx] = utility::ColorToDouble(r, g, b);
            }
            // X Y Z R G B
            else if (num_of_fields == 6 && sscanf(line_buffer, "%lf %lf %lf %d %d %d", &x, &y, &z, &r, &g, &b) == 6) {
                pointcloud.points_[idx] = Eigen::Vector3d(x, y, z);
                pointcloud.colors_[idx] = utility::ColorToDouble(r, g, b);
            }
            // X Y Z I
            else if (num_of_fields == 4 && sscanf(line_buffer, "%lf %lf %lf %lf", &x, &y, &z, &i) == 4) {
                pointcloud.points_[idx] = Eigen::Vector3d(x, y, z);
            }
            // X Y Z
            else if (num_of_fields == 3 && sscanf(line_buffer, "%lf %lf %lf", &x, &y, &z) == 3) {
                pointcloud.points_[idx] = Eigen::Vector3d(x, y, z);
            } else {
                LOGW("Read PTS failed at line: {}. ", line_buffer)
                return false;
            }

            idx++;
            if (idx % 1000 == 0) {
                reporter.Update(idx);
            }
        }

        reporter.Finish();
        return true;
    } catch (const std::exception &e) {
        LOGW("Read PTS failed with exception: {}", e.what())
        return false;
    }
}

bool WritePointCloudToPTS(const std::string &filename,
                          const geometry::PointCloud &pointcloud,
                          const WritePointCloudOption &params) {
    try {
        utility::filesystem::CFile file;
        if (!file.Open(filename, "w")) {
            LOGW("Write PTS failed: unable to open file: {}", filename)
            return false;
        }
        utility::CountingProgressReporter reporter(params.update_progress);
        reporter.SetTotal(pointcloud.points_.size());

        if (fprintf(file.GetFILE(), "%zu\r\n", (size_t)pointcloud.points_.size()) < 0) {
            LOGW("Write PTS failed: unable to write file: {}", filename)
            return false;
        }
        for (size_t i = 0; i < pointcloud.points_.size(); i++) {
            const auto &point = pointcloud.points_[i];
            if (!pointcloud.HasColors()) {
                if (fprintf(file.GetFILE(), "%.10f %.10f %.10f\r\n", point(0), point(1), point(2)) < 0) {
                    LOGW("Write PTS failed: unable to write file: {}", filename)
                    return false;
                }
            } else {
                auto color = utility::ColorToUint8(pointcloud.colors_[i]);
                if (fprintf(file.GetFILE(), "%.10f %.10f %.10f %.10f %d %d %d\r\n", point(0), point(1), point(2), 0.0,
                            (int)color(0), (int)color(1), (int)(color(2))) < 0) {
                    LOGW("Write PTS failed: unable to write file: {}", filename)
                    return false;
                }
            }
            if (i % 1000 == 0) {
                reporter.Update(i);
            }
        }
        reporter.Finish();
        return true;
    } catch (const std::exception &e) {
        LOGW("Write PTS failed with exception: {}", e.what())
        return false;
    }
}

}  // namespace vox::io
