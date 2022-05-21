//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "point_cloud_io.h"

#include <unordered_map>

#include "file_system.h"
#include "helper.h"
#include "logging.h"
#include "progress_reporters.h"

namespace vox::io {

static const std::unordered_map<
        std::string,
        std::function<bool(const std::string &, geometry::PointCloud &, const ReadPointCloudOption &)>>
        file_extension_to_pointcloud_read_function{
                {"xyz", ReadPointCloudFromXYZ}, {"xyzn", ReadPointCloudFromXYZN}, {"xyzrgb", ReadPointCloudFromXYZRGB},
                {"ply", ReadPointCloudFromPLY}, {"pcd", ReadPointCloudFromPCD},   {"pts", ReadPointCloudFromPTS},
        };

static const std::unordered_map<
        std::string,
        std::function<bool(const std::string &, const geometry::PointCloud &, const WritePointCloudOption &)>>
        file_extension_to_pointcloud_write_function{
                {"xyz", WritePointCloudToXYZ}, {"xyzn", WritePointCloudToXYZN}, {"xyzrgb", WritePointCloudToXYZRGB},
                {"ply", WritePointCloudToPLY}, {"pcd", WritePointCloudToPCD},   {"pts", WritePointCloudToPTS},
        };

std::shared_ptr<geometry::PointCloud> CreatePointCloudFromFile(const std::string &filename,
                                                               const std::string &format,
                                                               bool print_progress) {
    auto pointcloud = std::make_shared<geometry::PointCloud>();
    ReadPointCloud(filename, *pointcloud, ReadPointCloudOption(format, true, true, print_progress));
    return pointcloud;
}

bool ReadPointCloud(const std::string &filename, geometry::PointCloud &pointcloud, const ReadPointCloudOption &params) {
    std::string format = params.format;
    if (format == "auto") {
        format = utility::filesystem::GetFileExtensionInLowerCase(filename);
    }

    LOGD("Format {} File {}", params.format, filename)

    auto map_itr = file_extension_to_pointcloud_read_function.find(format);
    if (map_itr == file_extension_to_pointcloud_read_function.end()) {
        LOGW("Read geometry::PointCloud failed: unknown file extension for "
             "{} (format: {}).",
             filename, params.format)
        return false;
    }
    bool success = map_itr->second(filename, pointcloud, params);
    LOGD("Read geometry::PointCloud: {} vertices.", pointcloud.points_.size())
    if (params.remove_nan_points || params.remove_infinite_points) {
        pointcloud.RemoveNonFinitePoints(params.remove_nan_points, params.remove_infinite_points);
    }
    return success;
}
bool ReadPointCloud(const std::string &filename,
                    geometry::PointCloud &pointcloud,
                    const std::string &file_format,
                    bool remove_nan_points,
                    bool remove_infinite_points,
                    bool print_progress) {
    std::string format = file_format;
    if (format == "auto") {
        format = utility::filesystem::GetFileExtensionInLowerCase(filename);
    }

    ReadPointCloudOption p;
    p.format = format;
    p.remove_nan_points = remove_nan_points;
    p.remove_infinite_points = remove_infinite_points;
    utility::ConsoleProgressUpdater progress_updater(
            std::string("Reading ") + utility::ToUpper(format) + " file: " + filename, print_progress);
    p.update_progress = progress_updater;
    return ReadPointCloud(filename, pointcloud, p);
}

bool WritePointCloud(const std::string &filename,
                     const geometry::PointCloud &pointcloud,
                     const WritePointCloudOption &params) {
    std::string format = utility::filesystem::GetFileExtensionInLowerCase(filename);
    auto map_itr = file_extension_to_pointcloud_write_function.find(format);
    if (map_itr == file_extension_to_pointcloud_write_function.end()) {
        LOGW("Write geometry::PointCloud failed: unknown file extension {} "
             "for file {}.",
             format, filename)
        return false;
    }

    bool success = map_itr->second(filename, pointcloud, params);
    LOGD("Write geometry::PointCloud: {} vertices.", pointcloud.points_.size())
    return success;
}
bool WritePointCloud(const std::string &filename,
                     const geometry::PointCloud &pointcloud,
                     bool write_ascii /* = false*/,
                     bool compressed /* = false*/,
                     bool print_progress) {
    WritePointCloudOption p;
    p.write_ascii = WritePointCloudOption::IsAscii(write_ascii);
    p.compressed = WritePointCloudOption::Compressed(compressed);
    std::string format = utility::filesystem::GetFileExtensionInLowerCase(filename);
    utility::ConsoleProgressUpdater progress_updater(
            std::string("Writing ") + utility::ToUpper(format) + " file: " + filename, print_progress);
    p.update_progress = progress_updater;
    return WritePointCloud(filename, pointcloud, p);
}

}  // namespace vox::io
