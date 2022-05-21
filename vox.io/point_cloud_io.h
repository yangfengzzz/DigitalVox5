//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include <utility>

#include "point_cloud.h"

namespace vox::io {

/// Factory function to create a pointcloud from a file
/// Return an empty pointcloud if fail to read the file.
std::shared_ptr<geometry::PointCloud> CreatePointCloudFromFile(const std::string &filename,
                                                               const std::string &format = "auto",
                                                               bool print_progress = false);

/// \struct ReadPointCloudOption
/// \brief Optional parameters to ReadPointCloud
struct ReadPointCloudOption {
    explicit ReadPointCloudOption(
            // Attention: when you update the defaults, update the docstrings in
            // pybind/io/class_io.cpp
            std::string format = "auto",
            bool remove_nan_points = false,
            bool remove_infinite_points = false,
            bool print_progress = false,
            std::function<bool(double)> update_progress = {})
        : format(std::move(format)),
          remove_nan_points(remove_nan_points),
          remove_infinite_points(remove_infinite_points),
          print_progress(print_progress),
          update_progress(std::move(update_progress)){};
    explicit ReadPointCloudOption(std::function<bool(double)> up) : ReadPointCloudOption() {
        update_progress = std::move(up);
    };
    /// Specifies what format the contents of the file are (and what loader to
    /// use), default "auto" means to go off of file extension.
    std::string format;
    /// Whether to remove all points that have nan
    bool remove_nan_points;
    /// Whether to remove all points that have +-inf
    bool remove_infinite_points;
    /// Print progress to stdout about loading progress.
    /// Also see \p update_progress if you want to have your own progress
    /// indicators or to be able to cancel loading.
    bool print_progress;
    /// Callback to invoke as reading is progressing, parameter is percentage
    /// completion (0.-100.) return true indicates to continue loading, false
    /// means to try to stop loading and cleanup
    std::function<bool(double)> update_progress;
};

/// The general entrance for reading a PointCloud from a file
/// The function calls read functions based on the extension name of filename.
/// See \p ReadPointCloudOption for additional options you can pass.
/// \return return true if the read function is successful, false otherwise.
bool ReadPointCloud(const std::string &filename,
                    geometry::PointCloud &pointcloud,
                    const ReadPointCloudOption &params = ReadPointCloudOption());

/// \struct WritePointCloudOption
/// \brief Optional parameters to WritePointCloud
struct WritePointCloudOption {
    enum class IsAscii : bool { Binary = false, Ascii = true };
    enum class Compressed : bool { Uncompressed = false, Compressed = true };
    explicit WritePointCloudOption(
            // Attention: when you update the defaults, update the docstrings in
            // pybind/io/class_io.cpp
            IsAscii write_ascii = IsAscii::Binary,
            Compressed compressed = Compressed::Uncompressed,
            bool print_progress = false,
            std::function<bool(double)> update_progress = {})
        : write_ascii(write_ascii),
          compressed(compressed),
          print_progress(print_progress),
          update_progress(std::move(update_progress)){};
    // for compatibility
    explicit WritePointCloudOption(bool write_ascii,
                                   bool compressed = false,
                                   bool print_progress = false,
                                   std::function<bool(double)> update_progress = {})
        : write_ascii(IsAscii(write_ascii)),
          compressed(Compressed(compressed)),
          print_progress(print_progress),
          update_progress(std::move(update_progress)){};
    explicit WritePointCloudOption(std::function<bool(double)> up) : WritePointCloudOption() {
        update_progress = std::move(up);
    };
    /// Whether to save in Ascii or Binary.  Some savers are capable of doing
    /// either, other ignore this.
    IsAscii write_ascii;
    /// Whether to save Compressed or Uncompressed.  Currently, only PCD is
    /// capable of compressing, and only if using IsAscii::Binary, all other
    /// formats ignore this.
    Compressed compressed;
    /// Print progress to stdout about loading progress.  Also see
    /// \p update_progress if you want to have your own progress indicators or
    /// to be able to cancel loading.
    bool print_progress;
    /// Callback to invoke as reading is progressing, parameter is percentage
    /// completion (0.-100.) return true indicates to continue loading, false
    /// means to try to stop loading and cleanup
    std::function<bool(double)> update_progress;
};

/// The general entrance for writing a PointCloud to a file
/// The function calls write functions based on the extension name of filename.
/// See \p WritePointCloudOption for additional options you can pass.
/// \return return true if the write function is successful, false otherwise.
bool WritePointCloud(const std::string &filename,
                     const geometry::PointCloud &pointcloud,
                     const WritePointCloudOption &params = WritePointCloudOption());

bool ReadPointCloudFromXYZ(const std::string &filename,
                           geometry::PointCloud &pointcloud,
                           const ReadPointCloudOption &params);

bool WritePointCloudToXYZ(const std::string &filename,
                          const geometry::PointCloud &pointcloud,
                          const WritePointCloudOption &params);

bool ReadPointCloudFromXYZN(const std::string &filename,
                            geometry::PointCloud &pointcloud,
                            const ReadPointCloudOption &params);

bool WritePointCloudToXYZN(const std::string &filename,
                           const geometry::PointCloud &pointcloud,
                           const WritePointCloudOption &params);

bool ReadPointCloudFromXYZRGB(const std::string &filename,
                              geometry::PointCloud &pointcloud,
                              const ReadPointCloudOption &params);

bool WritePointCloudToXYZRGB(const std::string &filename,
                             const geometry::PointCloud &pointcloud,
                             const WritePointCloudOption &params);

bool ReadPointCloudFromPLY(const std::string &filename,
                           geometry::PointCloud &pointcloud,
                           const ReadPointCloudOption &params);

bool WritePointCloudToPLY(const std::string &filename,
                          const geometry::PointCloud &pointcloud,
                          const WritePointCloudOption &params);

bool ReadPointCloudFromPCD(const std::string &filename,
                           geometry::PointCloud &pointcloud,
                           const ReadPointCloudOption &params);

bool WritePointCloudToPCD(const std::string &filename,
                          const geometry::PointCloud &pointcloud,
                          const WritePointCloudOption &params);

bool ReadPointCloudFromPTS(const std::string &filename,
                           geometry::PointCloud &pointcloud,
                           const ReadPointCloudOption &params);

bool WritePointCloudToPTS(const std::string &filename,
                          const geometry::PointCloud &pointcloud,
                          const WritePointCloudOption &params);

}  // namespace vox::io
