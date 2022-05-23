//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

#include "vox.geometry/line_set.h"

namespace vox::io {

/// Factory function to create a lineset from a file.
/// \return return an empty lineset if fail to read the file.
std::shared_ptr<geometry::LineSet> CreateLineSetFromFile(const std::string &filename,
                                                         const std::string &format = "auto",
                                                         bool print_progress = false);

/// The general entrance for reading a LineSet from a file
/// The function calls read functions based on the extension name of filename.
/// \return return true if the read function is successful, false otherwise.
bool ReadLineSet(const std::string &filename,
                 geometry::LineSet &lineset,
                 const std::string &format = "auto",
                 bool print_progress = false);

/// The general entrance for writing a LineSet to a file
/// The function calls write functions based on the extension name of filename.
/// If the write function supports binary encoding and compression, the later
/// two parameters will be used. Otherwise they will be ignored.
/// \return return true if the write function is successful, false otherwise.
bool WriteLineSet(const std::string &filename,
                  const geometry::LineSet &lineset,
                  bool write_ascii = false,
                  bool compressed = false,
                  bool print_progress = false);

bool ReadLineSetFromPLY(const std::string &filename, geometry::LineSet &lineset, bool print_progress = false);

bool WriteLineSetToPLY(const std::string &filename,
                       const geometry::LineSet &lineset,
                       bool write_ascii = false,
                       bool compressed = false,
                       bool print_progress = false);

}  // namespace vox::io
