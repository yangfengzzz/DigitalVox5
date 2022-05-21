//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include <vector>

namespace vox::utility {

/// \brief Computes MD5 Hash for the given file.
/// \param file_path Path to the file.
std::string GetMD5(const std::string& file_path);

/// \brief Download a file from URL.
///
/// \param url File URL. The saved file name will be the last part of the URL.
/// \param md5 MD5 checksum of the file. This is required as the same
/// URL may point to different files over time.
/// \param prefix The file will be downloaded to `data_root/prefix`.
/// Typically we group data file by dataset, e.g., "kitti", "rgbd", etc. If
/// empty, the file will be downloaded to `data_root` directly.
/// \param data_root Open3D data root directory. If empty, the default data root
/// is used. The default data root is $HOME/open3d_data. For more information,
/// see vox::data::Dataset class.
/// \returns Path to downloaded file.
/// \throw std::runtime_error If the download fails.
std::string DownloadFromURL(const std::string& url,
                            const std::string& md5,
                            const std::string& prefix,
                            const std::string& data_root = "");

/// \brief Download a file from list of mirror URLs.
///
/// \param urls List of file mirror URLs for the same file. The saved
/// file name will be the last part of the URL.
/// \param md5 MD5 checksum of the file. This is required as the same URL may
/// point to different files over time.
/// \param prefix The file will be downloaded to `data_root/prefix`. Typically
/// we group data file by dataset, e.g., "kitti", "rgbd", etc. If empty, the
/// file will be downloaded to `data_root` directly.
/// \param data_root Open3D data root directory. If empty, the default data root
/// is used. The default data root is $HOME/open3d_data. For more information,
/// see vox::data::Dataset class.
/// \returns Path to downloaded file.
/// \throw std::runtime_error If the download fails.
std::string DownloadFromURL(const std::vector<std::string>& urls,
                            const std::string& md5,
                            const std::string& prefix,
                            const std::string& data_root = "");

}  // namespace vox::utility
