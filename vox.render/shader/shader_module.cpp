//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader/shader_module.h"

#include "logging.h"
#include "core/device.h"
#include "glsl_compiler.h"
#include "spirv_reflection.h"
#include "platform/filesystem.h"

namespace vox {
/**
 * @brief Pre-compiles project shader files to include header code
 * @param source The shader file
 * @returns A byte array of the final shader
 */
inline std::vector<std::string> precompile_shader(const std::string &source) {
    std::vector<std::string> final_file;
    
    auto lines = split(source, '\n');
    
    for (auto &line : lines) {
        if (line.find("#include \"") == 0) {
            // Include paths are relative to the base shader directory
            std::string include_path = line.substr(10);
            size_t last_quote = include_path.find('\"');
            if (!include_path.empty() && last_quote != std::string::npos) {
                include_path = include_path.substr(0, last_quote);
            }
            
            auto include_file = precompile_shader(fs::read_shader(include_path));
            for (auto &include_file_line : include_file) {
                final_file.push_back(include_file_line);
            }
        } else {
            final_file.push_back(line);
        }
    }
    
    return final_file;
}

inline std::vector<uint8_t> convert_to_bytes(std::vector<std::string> &lines) {
    std::vector<uint8_t> bytes;
    
    for (auto &line : lines) {
        line += "\n";
        std::vector<uint8_t> line_bytes(line.begin(), line.end());
        bytes.insert(bytes.end(), line_bytes.begin(), line_bytes.end());
    }
    
    return bytes;
}

ShaderModule::ShaderModule(Device &device, VkShaderStageFlagBits stage, const ShaderSource &glsl_source,
                           const std::string &entry_point, const ShaderVariant &shader_variant) :
device_{device},
stage_{stage},
entry_point_{entry_point} {
    debug_name_ = fmt::format("{} [variant {:X}] [entrypoint {}]",
                              glsl_source.get_filename(), shader_variant.get_id(), entry_point);
    
    // Compiling from GLSL source requires the entry point
    if (entry_point.empty()) {
        throw VulkanException{VK_ERROR_INITIALIZATION_FAILED};
    }
    
    auto &source = glsl_source.get_source();
    
    // Check if application is passing in GLSL source code to compile to SPIR-V
    if (source.empty()) {
        throw VulkanException{VK_ERROR_INITIALIZATION_FAILED};
    }
    
    // Precompile source into the final spirv bytecode
    auto glsl_final_source = precompile_shader(source);
    
    // Compile the GLSL source
    if (!GLSLCompiler::compile_to_spirv(stage, convert_to_bytes(glsl_final_source), entry_point, shader_variant,
                                        spirv_, info_log_)) {
        LOGE("Shader compilation failed for shader \"{}\"", glsl_source.get_filename())
        LOGE("{}", info_log_)
        throw VulkanException{VK_ERROR_INITIALIZATION_FAILED};
    }
    
    // Reflect all shader resources
    if (!SpirvReflection::reflect_shader_resources(stage, spirv_, resources_, shader_variant)) {
        throw VulkanException{VK_ERROR_INITIALIZATION_FAILED};
    }
    
    // Generate a unique id, determined by source and variant
    std::hash<std::string> hasher{};
    id_ = hasher(std::string{reinterpret_cast<const char *>(spirv_.data()),
        reinterpret_cast<const char *>(spirv_.data() + spirv_.size())});
}

ShaderModule::ShaderModule(ShaderModule &&other) noexcept:
device_{other.device_},
id_{other.id_},
stage_{other.stage_},
entry_point_{other.entry_point_},
debug_name_{other.debug_name_},
spirv_{other.spirv_},
resources_{other.resources_},
info_log_{other.info_log_} {
    other.stage_ = {};
}

size_t ShaderModule::get_id() const {
    return id_;
}

VkShaderStageFlagBits ShaderModule::get_stage() const {
    return stage_;
}

const std::string &ShaderModule::get_entry_point() const {
    return entry_point_;
}

const std::vector<ShaderResource> &ShaderModule::get_resources() const {
    return resources_;
}

const std::string &ShaderModule::get_info_log() const {
    return info_log_;
}

const std::vector<uint32_t> &ShaderModule::get_binary() const {
    return spirv_;
}

void ShaderModule::set_resource_mode(const std::string &resource_name, const ShaderResourceMode &resource_mode) {
    auto it = std::find_if(resources_.begin(), resources_.end(), [&resource_name](const ShaderResource &resource) {
        return resource.name == resource_name;
    });
    
    if (it != resources_.end()) {
        if (resource_mode == ShaderResourceMode::DYNAMIC) {
            if (it->type == ShaderResourceType::BUFFER_UNIFORM || it->type == ShaderResourceType::BUFFER_STORAGE) {
                it->mode = resource_mode;
            } else {
                LOGW("Resource `{}` does not support dynamic.", resource_name)
            }
        } else {
            it->mode = resource_mode;
        }
    } else {
        LOGW("Resource `{}` not found for shader.", resource_name)
    }
}

}        // namespace vox