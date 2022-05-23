//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

#include "vox.geometry/voxel_grid.h"

namespace vox::io {

/// Factory function to create a voxelgrid from a file.
/// \return return an empty voxelgrid if fail to read the file.
std::shared_ptr<geometry::VoxelGrid> CreateVoxelGridFromFile(const std::string &filename,
                                                             const std::string &format = "auto",
                                                             bool print_progress = false);

/// The general entrance for reading a VoxelGrid from a file
/// The function calls read functions based on the extension name of filename.
/// \return return true if the read function is successful, false otherwise.
bool ReadVoxelGrid(const std::string &filename,
                   geometry::VoxelGrid &voxelgrid,
                   const std::string &format = "auto",
                   bool print_progress = false);

/// The general entrance for writing a VoxelGrid to a file
/// The function calls write functions based on the extension name of filename.
/// If the write function supports binary encoding and compression, the later
/// two parameters will be used. Otherwise they will be ignored.
/// \return return true if the write function is successful, false otherwise.
bool WriteVoxelGrid(const std::string &filename,
                    const geometry::VoxelGrid &voxelgrid,
                    bool write_ascii = false,
                    bool compressed = false,
                    bool print_progress = false);

bool ReadVoxelGridFromPLY(const std::string &filename, geometry::VoxelGrid &voxelgrid, bool print_progress = false);

bool WriteVoxelGridToPLY(const std::string &filename,
                         const geometry::VoxelGrid &voxelgrid,
                         bool write_ascii = false,
                         bool compressed = false,
                         bool print_progress = false);

}  // namespace vox::io
