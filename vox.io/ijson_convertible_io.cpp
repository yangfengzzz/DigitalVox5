// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018-2021 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#include "ijson_convertible_io.h"

#include <unordered_map>

#include "file_system.h"
#include "logging.h"

namespace vox {

namespace {
using namespace io;

const std::unordered_map<std::string, std::function<bool(const std::string &, utility::IJsonConvertible &)>>
        file_extension_to_ijsonconvertible_read_function{
                {"json", ReadIJsonConvertibleFromJSON},
        };

const std::unordered_map<std::string, std::function<bool(const std::string &, const utility::IJsonConvertible &)>>
        file_extension_to_ijsonconvertible_write_function{
                {"json", WriteIJsonConvertibleToJSON},
        };

}  // unnamed namespace

namespace io {

bool ReadIJsonConvertible(const std::string &filename, utility::IJsonConvertible &object) {
    std::string filename_ext = utility::filesystem::GetFileExtensionInLowerCase(filename);
    if (filename_ext.empty()) {
        LOGW("Read utility::IJsonConvertible failed: unknown file "
             "extension.")
        return false;
    }
    auto map_itr = file_extension_to_ijsonconvertible_read_function.find(filename_ext);
    if (map_itr == file_extension_to_ijsonconvertible_read_function.end()) {
        LOGW("Read utility::IJsonConvertible failed: unknown file "
             "extension.")
        return false;
    }
    return map_itr->second(filename, object);
}

bool WriteIJsonConvertible(const std::string &filename, const utility::IJsonConvertible &object) {
    std::string filename_ext = utility::filesystem::GetFileExtensionInLowerCase(filename);
    if (filename_ext.empty()) {
        LOGW("Write utility::IJsonConvertible failed: unknown file "
             "extension.")
        return false;
    }
    auto map_itr = file_extension_to_ijsonconvertible_write_function.find(filename_ext);
    if (map_itr == file_extension_to_ijsonconvertible_write_function.end()) {
        LOGW("Write utility::IJsonConvertible failed: unknown file "
             "extension.")
        return false;
    }
    return map_itr->second(filename, object);
}

}  // namespace io
}  // namespace vox
