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

#pragma once

#include <algorithm>
#include <iterator>
#include <string>

#include "ijson_convertible.h"

namespace vox::io {

/// The general entrance for reading an IJsonConvertible from a file
/// The function calls read functions based on the extension name of filename.
/// \return return true if the read function is successful, false otherwise.
bool ReadIJsonConvertible(const std::string &filename, utility::IJsonConvertible &object);

/// The general entrance for writing an IJsonConvertible to a file
/// The function calls write functions based on the extension name of filename.
/// \return return true if the write function is successful, false otherwise.
bool WriteIJsonConvertible(const std::string &filename, const utility::IJsonConvertible &object);

bool ReadIJsonConvertibleFromJSON(const std::string &filename, utility::IJsonConvertible &object);

bool WriteIJsonConvertibleToJSON(const std::string &filename, const utility::IJsonConvertible &object);

bool ReadIJsonConvertibleFromJSONString(const std::string &json_string, utility::IJsonConvertible &object);

bool WriteIJsonConvertibleToJSONString(std::string &json_string, const utility::IJsonConvertible &object);

/// String to and from enum mapping, based on
/// https://github.com/nlohmann/json/blob/master/include/nlohmann/detail/macro_scope.hpp
/// (MIT license)
/// If you have an enum:
/// enum IMAGE_FORMAT {FORMAT_PNG,  FORMAT_JPG};
/// Use as STRINGIFY_ENUM(IMAGE_FORMAT, {
///      {FORMAT_INVALID, nullptr},
///      {FORMAT_PNG, "png"},
///      {FORMAT_JPG, "jpg"}
///      })
/// in the cpp file and
/// DECLARE_STRINGIFY_ENUM(IMAGE_FORMAT)
/// in the header file. This creates the functions
/// - enum_to_string(const ENUM_TYPE &e) -> std::string
/// - enum_from_string(const std::string &str, ENUM_TYPE &e) -> void
/// for conversion between the enum and string. Invalid string values are mapped
/// to the first specified option in the macro.
#define DECLARE_STRINGIFY_ENUM(ENUM_TYPE)    \
    std::string enum_to_string(ENUM_TYPE e); \
    void enum_from_string(const std::string &str, (ENUM_TYPE) &e);

#define STRINGIFY_ENUM(ENUM_TYPE, ...)                                                                               \
    std::string enum_to_string(ENUM_TYPE e) {                                                                        \
        static_assert(std::is_enum<ENUM_TYPE>::value, #ENUM_TYPE " must be an enum!");                               \
        static const std::pair<ENUM_TYPE, std::string> m[] = __VA_ARGS__;                                            \
        auto it = std::find_if(                                                                                      \
                std::begin(m), std::end(m),                                                                          \
                [e](const std::pair<ENUM_TYPE, std::string> &es_pair) -> bool { return es_pair.first == e; });       \
        return ((it != std::end(m)) ? it : std::begin(m))->second;                                                   \
    }                                                                                                                \
    void enum_from_string(const std::string &str, ENUM_TYPE &e) {                                                    \
        static_assert(std::is_enum<ENUM_TYPE>::value, #ENUM_TYPE " must be an enum!");                               \
        static const std::pair<ENUM_TYPE, std::string> m[] = __VA_ARGS__;                                            \
        auto it = std::find_if(                                                                                      \
                std::begin(m), std::end(m),                                                                          \
                [&str](const std::pair<ENUM_TYPE, std::string> &es_pair) -> bool { return es_pair.second == str; }); \
        e = ((it != std::end(m)) ? it : std::begin(m))->first;                                                       \
        LOGD("{} -> {}", str, e);                                                                       \
    }

}  // namespace vox
