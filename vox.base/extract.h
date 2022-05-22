//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

namespace vox::utility {

/// \brief Function to extract compressed files.
/// \param file_path Path to file. Example: "/path/to/file/file.zip"
/// \param extract_dir Directory path where the file will be extracted to.
void Extract(const std::string& file_path, const std::string& extract_dir);

}  // namespace vox::utility
