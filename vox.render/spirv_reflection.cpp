//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "spirv_reflection.h"

namespace vox {
namespace {
template<ShaderResourceType T>
inline void read_shader_resource(const spirv_cross::Compiler &compiler,
                                 VkShaderStageFlagBits stage,
                                 std::vector<ShaderResource> &resources,
                                 const ShaderVariant &variant) {
    LOGE("Not implemented! Read shader resources of type.")
}

template<spv::Decoration T>
inline void read_resource_decoration(const spirv_cross::Compiler & /*compiler*/,
                                     const spirv_cross::Resource & /*resource*/,
                                     ShaderResource & /*shader_resource*/,
                                     const ShaderVariant & /* variant */) {
    LOGE("Not implemented! Read resources decoration of type.")
}

template<>
inline void read_resource_decoration<spv::DecorationLocation>(const spirv_cross::Compiler &compiler,
                                                              const spirv_cross::Resource &resource,
                                                              ShaderResource &shader_resource,
                                                              const ShaderVariant &variant) {
    shader_resource.location = compiler.get_decoration(resource.id, spv::DecorationLocation);
}

template<>
inline void read_resource_decoration<spv::DecorationDescriptorSet>(const spirv_cross::Compiler &compiler,
                                                                   const spirv_cross::Resource &resource,
                                                                   ShaderResource &shader_resource,
                                                                   const ShaderVariant &variant) {
    shader_resource.set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
}

template<>
inline void read_resource_decoration<spv::DecorationBinding>(const spirv_cross::Compiler &compiler,
                                                             const spirv_cross::Resource &resource,
                                                             ShaderResource &shader_resource,
                                                             const ShaderVariant &variant) {
    shader_resource.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
}

template<>
inline void read_resource_decoration<spv::DecorationInputAttachmentIndex>(const spirv_cross::Compiler &compiler,
                                                                          const spirv_cross::Resource &resource,
                                                                          ShaderResource &shader_resource,
                                                                          const ShaderVariant &variant) {
    shader_resource.input_attachment_index = compiler.get_decoration(resource.id,
                                                                     spv::DecorationInputAttachmentIndex);
}

template<>
inline void read_resource_decoration<spv::DecorationNonWritable>(const spirv_cross::Compiler &compiler,
                                                                 const spirv_cross::Resource &resource,
                                                                 ShaderResource &shader_resource,
                                                                 const ShaderVariant &variant) {
    shader_resource.qualifiers |= ShaderResourceQualifiers::NON_WRITABLE;
}

template<>
inline void read_resource_decoration<spv::DecorationNonReadable>(const spirv_cross::Compiler &compiler,
                                                                 const spirv_cross::Resource &resource,
                                                                 ShaderResource &shader_resource,
                                                                 const ShaderVariant &variant) {
    shader_resource.qualifiers |= ShaderResourceQualifiers::NON_READABLE;
}

inline void read_resource_vec_size(const spirv_cross::Compiler &compiler,
                                   const spirv_cross::Resource &resource,
                                   ShaderResource &shader_resource,
                                   const ShaderVariant &variant) {
    const auto &spirv_type = compiler.get_type_from_variable(resource.id);
    
    shader_resource.vec_size = spirv_type.vecsize;
    shader_resource.columns = spirv_type.columns;
}

inline void read_resource_array_size(const spirv_cross::Compiler &compiler,
                                     const spirv_cross::Resource &resource,
                                     ShaderResource &shader_resource,
                                     const ShaderVariant &variant) {
    const auto &spirv_type = compiler.get_type_from_variable(resource.id);
    
    shader_resource.array_size = !spirv_type.array.empty() ? spirv_type.array[0] : 1;
}

inline void read_resource_size(const spirv_cross::Compiler &compiler,
                               const spirv_cross::Resource &resource,
                               ShaderResource &shader_resource,
                               const ShaderVariant &variant) {
    const auto &spirv_type = compiler.get_type_from_variable(resource.id);
    
    size_t array_size = 0;
    if (variant.get_runtime_array_sizes().count(resource.name) != 0) {
        array_size = variant.get_runtime_array_sizes().at(resource.name);
    }
    
    shader_resource.size = to_u32(compiler.get_declared_struct_size_runtime_array(spirv_type, array_size));
}

inline void read_resource_size(const spirv_cross::Compiler &compiler,
                               const spirv_cross::SPIRConstant &constant,
                               ShaderResource &shader_resource,
                               const ShaderVariant &variant) {
    auto spirv_type = compiler.get_type(constant.constant_type);
    
    switch (spirv_type.basetype) {
        case spirv_cross::SPIRType::BaseType::Boolean:
        case spirv_cross::SPIRType::BaseType::Char:
        case spirv_cross::SPIRType::BaseType::Int:
        case spirv_cross::SPIRType::BaseType::UInt:
        case spirv_cross::SPIRType::BaseType::Float:
            shader_resource.size = 4;
            break;
        case spirv_cross::SPIRType::BaseType::Int64:
        case spirv_cross::SPIRType::BaseType::UInt64:
        case spirv_cross::SPIRType::BaseType::Double:
            shader_resource.size = 8;
            break;
        default:
            shader_resource.size = 0;
            break;
    }
}

template<>
inline void read_shader_resource<ShaderResourceType::INPUT>(const spirv_cross::Compiler &compiler,
															VkShaderStageFlagBits stage,
															std::vector<ShaderResource> &resources,
															const ShaderVariant &variant) {
    auto input_resources = compiler.get_shader_resources().stage_inputs;
    
    for (auto &resource: input_resources) {
        ShaderResource shader_resource{};
        shader_resource.type = ShaderResourceType::INPUT;
        shader_resource.stages = stage;
        shader_resource.name = resource.name;
        
        read_resource_vec_size(compiler, resource, shader_resource, variant);
        read_resource_array_size(compiler, resource, shader_resource, variant);
        read_resource_decoration<spv::DecorationLocation>(compiler, resource, shader_resource, variant);
        
        resources.push_back(shader_resource);
    }
}

template<>
inline void read_shader_resource<ShaderResourceType::INPUT_ATTACHMENT>(const spirv_cross::Compiler &compiler,
																	   VkShaderStageFlagBits /*stage*/,
																	   std::vector<ShaderResource> &resources,
																	   const ShaderVariant &variant) {
    auto subpass_resources = compiler.get_shader_resources().subpass_inputs;
    
    for (auto &resource: subpass_resources) {
        ShaderResource shader_resource{};
        shader_resource.type = ShaderResourceType::INPUT_ATTACHMENT;
        shader_resource.stages = VK_SHADER_STAGE_FRAGMENT_BIT;
        shader_resource.name = resource.name;
        
        read_resource_array_size(compiler, resource, shader_resource, variant);
        read_resource_decoration<spv::DecorationInputAttachmentIndex>(compiler, resource, shader_resource,
                                                                      variant);
        read_resource_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
        read_resource_decoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);
        
        resources.push_back(shader_resource);
    }
}

template<>
inline void read_shader_resource<ShaderResourceType::OUTPUT>(const spirv_cross::Compiler &compiler,
															 VkShaderStageFlagBits stage,
															 std::vector<ShaderResource> &resources,
															 const ShaderVariant &variant) {
    auto output_resources = compiler.get_shader_resources().stage_outputs;
    
    for (auto &resource: output_resources) {
        ShaderResource shader_resource{};
        shader_resource.type = ShaderResourceType::OUTPUT;
        shader_resource.stages = stage;
        shader_resource.name = resource.name;
        
        read_resource_array_size(compiler, resource, shader_resource, variant);
        read_resource_vec_size(compiler, resource, shader_resource, variant);
        read_resource_decoration<spv::DecorationLocation>(compiler, resource, shader_resource, variant);
        
        resources.push_back(shader_resource);
    }
}

template<>
inline void read_shader_resource<ShaderResourceType::IMAGE>(const spirv_cross::Compiler &compiler,
															VkShaderStageFlagBits stage,
															std::vector<ShaderResource> &resources,
															const ShaderVariant &variant) {
    auto image_resources = compiler.get_shader_resources().separate_images;
    
    for (auto &resource: image_resources) {
        ShaderResource shader_resource{};
        shader_resource.type = ShaderResourceType::IMAGE;
        shader_resource.stages = stage;
        shader_resource.name = resource.name;
        
        read_resource_array_size(compiler, resource, shader_resource, variant);
        read_resource_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
        read_resource_decoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);
        
        resources.push_back(shader_resource);
    }
}

template<>
inline void read_shader_resource<ShaderResourceType::IMAGE_SAMPLER>(const spirv_cross::Compiler &compiler,
																	VkShaderStageFlagBits stage,
																	std::vector<ShaderResource> &resources,
																	const ShaderVariant &variant) {
    auto image_resources = compiler.get_shader_resources().sampled_images;
    
    for (auto &resource: image_resources) {
        ShaderResource shader_resource{};
        shader_resource.type = ShaderResourceType::IMAGE_SAMPLER;
        shader_resource.stages = stage;
        shader_resource.name = resource.name;
        
        read_resource_array_size(compiler, resource, shader_resource, variant);
        read_resource_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
        read_resource_decoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);
        
        resources.push_back(shader_resource);
    }
}

template<>
inline void read_shader_resource<ShaderResourceType::IMAGE_STORAGE>(const spirv_cross::Compiler &compiler,
																	VkShaderStageFlagBits stage,
																	std::vector<ShaderResource> &resources,
																	const ShaderVariant &variant) {
    auto storage_resources = compiler.get_shader_resources().storage_images;
    
    for (auto &resource: storage_resources) {
        ShaderResource shader_resource{};
        shader_resource.type = ShaderResourceType::IMAGE_STORAGE;
        shader_resource.stages = stage;
        shader_resource.name = resource.name;
        
        read_resource_array_size(compiler, resource, shader_resource, variant);
        read_resource_decoration<spv::DecorationNonReadable>(compiler, resource, shader_resource, variant);
        read_resource_decoration<spv::DecorationNonWritable>(compiler, resource, shader_resource, variant);
        read_resource_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
        read_resource_decoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);
        
        resources.push_back(shader_resource);
    }
}

template<>
inline void read_shader_resource<ShaderResourceType::SAMPLER>(const spirv_cross::Compiler &compiler,
															  VkShaderStageFlagBits stage,
															  std::vector<ShaderResource> &resources,
															  const ShaderVariant &variant) {
    auto sampler_resources = compiler.get_shader_resources().separate_samplers;
    
    for (auto &resource: sampler_resources) {
        ShaderResource shader_resource{};
        shader_resource.type = ShaderResourceType::SAMPLER;
        shader_resource.stages = stage;
        shader_resource.name = resource.name;
        
        read_resource_array_size(compiler, resource, shader_resource, variant);
        read_resource_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
        read_resource_decoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);
        
        resources.push_back(shader_resource);
    }
}

template<>
inline void read_shader_resource<ShaderResourceType::BUFFER_UNIFORM>(const spirv_cross::Compiler &compiler,
																	 VkShaderStageFlagBits stage,
																	 std::vector<ShaderResource> &resources,
																	 const ShaderVariant &variant) {
    auto uniform_resources = compiler.get_shader_resources().uniform_buffers;
    
    for (auto &resource: uniform_resources) {
        ShaderResource shader_resource{};
        shader_resource.type = ShaderResourceType::BUFFER_UNIFORM;
        shader_resource.stages = stage;
        shader_resource.name = resource.name;
        
        read_resource_size(compiler, resource, shader_resource, variant);
        read_resource_array_size(compiler, resource, shader_resource, variant);
        read_resource_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
        read_resource_decoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);
        
        resources.push_back(shader_resource);
    }
}

template<>
inline void read_shader_resource<ShaderResourceType::BUFFER_STORAGE>(const spirv_cross::Compiler &compiler,
																	 VkShaderStageFlagBits stage,
																	 std::vector<ShaderResource> &resources,
																	 const ShaderVariant &variant) {
    auto storage_resources = compiler.get_shader_resources().storage_buffers;
    
    for (auto &resource: storage_resources) {
        ShaderResource shader_resource;
        shader_resource.type = ShaderResourceType::BUFFER_STORAGE;
        shader_resource.stages = stage;
        shader_resource.name = resource.name;
        
        read_resource_size(compiler, resource, shader_resource, variant);
        read_resource_array_size(compiler, resource, shader_resource, variant);
        read_resource_decoration<spv::DecorationNonReadable>(compiler, resource, shader_resource, variant);
        read_resource_decoration<spv::DecorationNonWritable>(compiler, resource, shader_resource, variant);
        read_resource_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
        read_resource_decoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);
        
        resources.push_back(shader_resource);
    }
}
}        // namespace

bool SPIRVReflection::reflect_shader_resources(VkShaderStageFlagBits stage, const std::vector<uint32_t> &spirv,
                                               std::vector<ShaderResource> &resources,
                                               const ShaderVariant &variant) {
    spirv_cross::CompilerGLSL compiler{spirv};
    
    auto opts = compiler.get_common_options();
    opts.enable_420pack_extension = true;
    
    compiler.set_common_options(opts);
    
    parse_shader_resources(compiler, stage, resources, variant);
    parse_push_constants(compiler, stage, resources, variant);
    parse_specialization_constants(compiler, stage, resources, variant);
    
    return true;
}

void SPIRVReflection::parse_shader_resources(const spirv_cross::Compiler &compiler, VkShaderStageFlagBits stage,
                                             std::vector<ShaderResource> &resources, const ShaderVariant &variant) {
    read_shader_resource<ShaderResourceType::INPUT>(compiler, stage, resources, variant);
    read_shader_resource<ShaderResourceType::INPUT_ATTACHMENT>(compiler, stage, resources, variant);
    read_shader_resource<ShaderResourceType::OUTPUT>(compiler, stage, resources, variant);
    read_shader_resource<ShaderResourceType::IMAGE>(compiler, stage, resources, variant);
    read_shader_resource<ShaderResourceType::IMAGE_SAMPLER>(compiler, stage, resources, variant);
    read_shader_resource<ShaderResourceType::IMAGE_STORAGE>(compiler, stage, resources, variant);
    read_shader_resource<ShaderResourceType::SAMPLER>(compiler, stage, resources, variant);
    read_shader_resource<ShaderResourceType::BUFFER_UNIFORM>(compiler, stage, resources, variant);
    read_shader_resource<ShaderResourceType::BUFFER_STORAGE>(compiler, stage, resources, variant);
}

void SPIRVReflection::parse_push_constants(const spirv_cross::Compiler &compiler, VkShaderStageFlagBits stage,
                                           std::vector<ShaderResource> &resources, const ShaderVariant &variant) {
    auto shader_resources = compiler.get_shader_resources();
    
    for (auto &resource: shader_resources.push_constant_buffers) {
        const auto &spivr_type = compiler.get_type_from_variable(resource.id);
        
        std::uint32_t offset = std::numeric_limits<std::uint32_t>::max();
        
        for (auto i = 0U; i < spivr_type.member_types.size(); ++i) {
            auto mem_offset = compiler.get_member_decoration(spivr_type.self, i, spv::DecorationOffset);
            
            offset = std::min(offset, mem_offset);
        }
        
        ShaderResource shader_resource{};
        shader_resource.type = ShaderResourceType::PUSH_CONSTANT;
        shader_resource.stages = stage;
        shader_resource.name = resource.name;
        shader_resource.offset = offset;
        
        read_resource_size(compiler, resource, shader_resource, variant);
        
        shader_resource.size -= shader_resource.offset;
        
        resources.push_back(shader_resource);
    }
}

void
SPIRVReflection::parse_specialization_constants(const spirv_cross::Compiler &compiler, VkShaderStageFlagBits stage,
                                                std::vector<ShaderResource> &resources,
                                                const ShaderVariant &variant) {
    auto specialization_constants = compiler.get_specialization_constants();
    
    for (auto &resource: specialization_constants) {
        auto &spirv_value = compiler.get_constant(resource.id);
        
        ShaderResource shader_resource{};
        shader_resource.type = ShaderResourceType::SPECIALIZATION_CONSTANT;
        shader_resource.stages = stage;
        shader_resource.name = compiler.get_name(resource.id);
        shader_resource.offset = 0;
        shader_resource.constant_id = resource.constant_id;
        
        read_resource_size(compiler, spirv_value, shader_resource, variant);
        
        resources.push_back(shader_resource);
    }
}


}        // namespace vox
