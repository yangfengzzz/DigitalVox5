//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

#include "../../arc.io/point_cloud_io.h"
#include "geometry/point_cloud.h"

namespace vox {
namespace t {
namespace io {

using arc::io::ReadPointCloudOption;
using arc::io::WritePointCloudOption;

/// Factory function to create a pointcloud from a file
/// Return an empty pointcloud if fail to read the file.
std::shared_ptr<geometry::PointCloud> CreatePointCloudFromFile(const std::string &filename,
                                                               const std::string &format = "auto",
                                                               bool print_progress = false);

/// The general entrance for reading a PointCloud from a file
/// The function calls read functions based on the extension name of filename.
/// See \p ReadPointCloudOption for additional options you can pass.
/// \return return true if the read function is successful, false otherwise.
bool ReadPointCloud(const std::string &filename,
                    geometry::PointCloud &pointcloud,
                    const ReadPointCloudOption &params = {});

/// The general entrance for writing a PointCloud to a file
/// The function calls write functions based on the extension name of filename.
/// See \p WritePointCloudOption for additional options you can pass.
/// \return return true if the write function is successful, false otherwise.
bool WritePointCloud(const std::string &filename,
                     const geometry::PointCloud &pointcloud,
                     const WritePointCloudOption &params = {});

bool ReadPointCloudFromNPZ(const std::string &filename,
                           geometry::PointCloud &pointcloud,
                           const ReadPointCloudOption &params);

bool WritePointCloudToNPZ(const std::string &filename,
                          const geometry::PointCloud &pointcloud,
                          const WritePointCloudOption &params);

bool ReadPointCloudFromXYZI(const std::string &filename,
                            geometry::PointCloud &pointcloud,
                            const ReadPointCloudOption &params);

bool WritePointCloudToXYZI(const std::string &filename,
                           const geometry::PointCloud &pointcloud,
                           const WritePointCloudOption &params);

bool ReadPointCloudFromPCD(const std::string &filename,
                           geometry::PointCloud &pointcloud,
                           const ReadPointCloudOption &params);

bool WritePointCloudToPCD(const std::string &filename,
                          const geometry::PointCloud &pointcloud,
                          const WritePointCloudOption &params);

bool ReadPointCloudFromPLY(const std::string &filename,
                           geometry::PointCloud &pointcloud,
                           const ReadPointCloudOption &params);

bool WritePointCloudToPLY(const std::string &filename,
                          const geometry::PointCloud &pointcloud,
                          const WritePointCloudOption &params);

bool ReadPointCloudFromPTS(const std::string &filename,
                           geometry::PointCloud &pointcloud,
                           const ReadPointCloudOption &params);

bool WritePointCloudToPTS(const std::string &filename,
                          const geometry::PointCloud &pointcloud,
                          const WritePointCloudOption &params);

}  // namespace io
}  // namespace t
}  // namespace vox
