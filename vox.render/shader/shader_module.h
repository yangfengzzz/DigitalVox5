//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vk_common.h"
#include "shader_variant.h"
#include "shader_source.h"

#if defined(VK_USE_PLATFORM_XLIB_KHR)
#	undef None
#endif

namespace vox {
class Device;

/// Types of shader resources
enum class ShaderResourceType {
    INPUT,
    INPUT_ATTACHMENT,
    OUTPUT,
    IMAGE,
    IMAGE_SAMPLER,
    IMAGE_STORAGE,
    SAMPLER,
    BUFFER_UNIFORM,
    BUFFER_STORAGE,
    PUSH_CONSTANT,
    SPECIALIZATION_CONSTANT,
    ALL
};

/// This determines the type and method of how descriptor set should be created and bound
enum class ShaderResourceMode {
    STATIC,
    DYNAMIC,
    UPDATE_AFTER_BIND
};

/// A bitmask of qualifiers applied to a resource
struct ShaderResourceQualifiers {
    enum : uint32_t {
        NONE = 0,
        NON_READABLE = 1,
        NON_WRITABLE = 2,
    };
};

/// Store shader resource data.
/// Used by the shader module.
struct ShaderResource {
    VkShaderStageFlags stages;
    
    ShaderResourceType type;
    
    ShaderResourceMode mode;
    
    uint32_t set;
    
    uint32_t binding;
    
    uint32_t location;
    
    uint32_t input_attachment_index;
    
    uint32_t vec_size;
    
    uint32_t columns;
    
    uint32_t array_size;
    
    uint32_t offset;
    
    uint32_t size;
    
    uint32_t constant_id;
    
    uint32_t qualifiers;
    
    std::string name;
};

/**
 * @brief Contains shader code, with an entry point, for a specific shader stage.
 * It is needed by a PipelineLayout to create a Pipeline.
 * ShaderModule can do auto-pairing between shader code and textures.
 * The low level code can change bindings, just keeping the name of the texture.
 * Variants for each texture are also generated, such as HAS_BASE_COLOR_TEX.
 * It works similarly for attribute locations. A current limitation is that only set 0
 * is considered. Uniform buffers are currently hardcoded as well.
 */
class ShaderModule {
public:
    ShaderModule(Device &device,
                 VkShaderStageFlagBits stage,
                 const ShaderSource &glsl_source,
                 const std::string &entry_point,
                 const ShaderVariant &shader_variant);
    
    ShaderModule(const ShaderModule &) = delete;
    
    ShaderModule(ShaderModule &&other) noexcept;
    
    ShaderModule &operator=(const ShaderModule &) = delete;
    
    ShaderModule &operator=(ShaderModule &&) = delete;
    
    [[nodiscard]] size_t get_id() const;
    
    [[nodiscard]] VkShaderStageFlagBits get_stage() const;
    
    [[nodiscard]] const std::string &get_entry_point() const;
    
    [[nodiscard]] const std::vector<ShaderResource> &get_resources() const;
    
    [[nodiscard]] const std::string &get_info_log() const;
    
    [[nodiscard]] const std::vector<uint32_t> &get_binary() const;
    
    [[nodiscard]] inline const std::string &get_debug_name() const {
        return debug_name_;
    }
    
    inline void set_debug_name(const std::string &name) {
        debug_name_ = name;
    }
    
    /**
     * @brief Flags a resource to use a different method of being bound to the shader
     * @param resource_name The name of the shader resource
     * @param resource_mode The mode of how the shader resource will be bound
     */
    void set_resource_mode(const std::string &resource_name, const ShaderResourceMode &resource_mode);
    
private:
    Device &device_;
    
    /// Shader unique id
    size_t id_;
    
    /// Stage of the shader (vertex, fragment, etc)
    VkShaderStageFlagBits stage_{};
    
    /// Name of the main function
    std::string entry_point_;
    
    /// Human-readable name for the shader
    std::string debug_name_;
    
    /// Compiled source
    std::vector<uint32_t> spirv_;
    
    std::vector<ShaderResource> resources_;
    
    std::string info_log_;
};

}        // namespace vox