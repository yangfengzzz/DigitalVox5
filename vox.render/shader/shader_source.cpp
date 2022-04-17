//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader_source.h"
#include "platform/filesystem.h"

namespace vox {
ShaderSource::ShaderSource(const std::string &filename) :
filename_{filename},
source_{fs::read_shader(filename)} {
    std::hash<std::string> hasher{};
    id_ = hasher(std::string{source_.cbegin(), source_.cend()});
}

size_t ShaderSource::get_id() const {
    return id_;
}

const std::string &ShaderSource::get_filename() const {
    return filename_;
}

void ShaderSource::set_source(const std::string &source) {
    source_ = source;
    std::hash<std::string> hasher{};
    id_ = hasher(std::string{source_.cbegin(), source_.cend()});
}

const std::string &ShaderSource::get_source() const {
    return source_;
}

}