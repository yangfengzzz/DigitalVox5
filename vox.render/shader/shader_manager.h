//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "shader_source.h"
#include "singleton.h"

namespace vox {
class ShaderManager : public Singleton<ShaderManager> {
public:
    static ShaderManager &get_singleton();
    
    static ShaderManager *get_singleton_ptr();
    
    /**
     * @brief Loads shader source
     */
    std::shared_ptr<ShaderSource> load_shader(const std::string &file);
    
    void collect_garbage();
    
private:
    std::unordered_map<std::string, std::shared_ptr<ShaderSource>> shader_pool_;
};

template<> inline ShaderManager *Singleton<ShaderManager>::ms_singleton_{nullptr};

}
