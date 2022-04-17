//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader_variant.h"

namespace vox {
ShaderVariant::ShaderVariant(std::string &&preamble, std::vector<std::string> &&processes) :
preamble_{std::move(preamble)},
processes_{std::move(processes)} {
    update_id();
}

size_t ShaderVariant::get_id() const {
    return id_;
}

void ShaderVariant::add_definitions(const std::vector<std::string> &definitions) {
    for (auto &definition : definitions) {
        add_define(definition);
    }
}

void ShaderVariant::add_define(const std::string &def) {
    processes_.push_back("D" + def);
    
    std::string tmp_def = def;
    
    // The "=" needs to turn into a space
    size_t pos_equal = tmp_def.find_first_of('=');
    if (pos_equal != std::string::npos) {
        tmp_def[pos_equal] = ' ';
    }
    
    preamble_.append("#define " + tmp_def + "\n");
    
    update_id();
}

void ShaderVariant::add_undefine(const std::string &undef) {
    processes_.push_back("U" + undef);
    
    preamble_.append("#undef " + undef + "\n");
    
    update_id();
}

void ShaderVariant::add_runtime_array_size(const std::string &runtime_array_name, size_t size) {
    if (runtime_array_sizes_.find(runtime_array_name) == runtime_array_sizes_.end()) {
        runtime_array_sizes_.insert({runtime_array_name, size});
    } else {
        runtime_array_sizes_[runtime_array_name] = size;
    }
}

void ShaderVariant::set_runtime_array_sizes(const std::unordered_map<std::string, size_t> &sizes) {
    runtime_array_sizes_ = sizes;
}

const std::string &ShaderVariant::get_preamble() const {
    return preamble_;
}

const std::vector<std::string> &ShaderVariant::get_processes() const {
    return processes_;
}

const std::unordered_map<std::string, size_t> &ShaderVariant::get_runtime_array_sizes() const {
    return runtime_array_sizes_;
}

void ShaderVariant::clear() {
    preamble_.clear();
    processes_.clear();
    runtime_array_sizes_.clear();
    update_id();
}

void ShaderVariant::update_id() {
    std::hash<std::string> hasher{};
    id_ = hasher(preamble_);
}

}
