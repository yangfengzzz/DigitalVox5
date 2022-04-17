//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "helpers.h"

namespace vox {
class ShaderSource {
public:
    ShaderSource() = default;
    
    explicit ShaderSource(const std::string &filename);
    
    [[nodiscard]] size_t get_id() const;
    
    [[nodiscard]] const std::string &get_filename() const;
    
    void set_source(const std::string &source);
    
    [[nodiscard]] const std::string &get_source() const;
    
private:
    size_t id_{};
    
    std::string filename_;
    
    std::string source_;
};

}
