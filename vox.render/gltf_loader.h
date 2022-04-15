//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>
#include <mutex>

#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_EXTERNAL_IMAGE

#include <tiny_gltf.h>

#include "timer.h"

#define KHR_LIGHTS_PUNCTUAL_EXTENSION "KHR_lights_punctual"

namespace vox {
class Device;

namespace sg {
class Camera;

class Image;

class Light;

class Mesh;

class Node;

class PBRMaterial;

class Sampler;

class Scene;

class SubMesh;

class Texture;
}        // namespace sg


/// Read a gltf file and return a scene object. Converts the gltf objects
/// to our internal scene implementation. Mesh data is copied to vulkan buffers and
/// images are loaded from the folder of gltf file to vulkan images.
class GLTFLoader {
public:
    explicit GLTFLoader(Device const &device);
    
    virtual ~GLTFLoader() = default;
    
    //    std::unique_ptr<sg::Scene> read_scene_from_file(const std::string &file_name, int scene_index = -1);
    
    /**
     * @brief Loads the first model from a GLTF file for use in simpler samples
     *        makes use of the Vertex struct in vulkan_example_base.h
     */
    std::unique_ptr<sg::SubMesh> read_model_from_file(const std::string &file_name, uint32_t index);
    
protected:
    //    virtual std::unique_ptr<sg::Node> parse_node(const tinygltf::Node &gltf_node, size_t index) const;
    //
    //    virtual std::unique_ptr<sg::Camera> parse_camera(const tinygltf::Camera &gltf_camera) const;
    //
    //    virtual std::unique_ptr<sg::Mesh> parse_mesh(const tinygltf::Mesh &gltf_mesh) const;
    //
    //    virtual std::unique_ptr<sg::PBRMaterial> parse_material(const tinygltf::Material &gltf_material) const;
    //
    //    virtual std::unique_ptr<sg::Image> parse_image(tinygltf::Image &gltf_image) const;
    //
    //    virtual std::unique_ptr<sg::Sampler> parse_sampler(const tinygltf::Sampler &gltf_sampler) const;
    //
    //    virtual std::unique_ptr<sg::Texture> parse_texture(const tinygltf::Texture &gltf_texture) const;
    //
    //    virtual std::unique_ptr<sg::PBRMaterial> create_default_material();
    //
    //    virtual std::unique_ptr<sg::Sampler> create_default_sampler();
    //
    //    virtual std::unique_ptr<sg::Camera> create_default_camera();
    //
    //    /**
    //     * @brief Parses and returns a list of scene graph lights from the KHR_lights_punctual extension
    //     */
    //    std::vector<std::unique_ptr<sg::Light>> parse_khr_lights_punctual();
    
    /**
     * @brief Checks if the GLTFLoader supports an extension, and that it is present in the glTF file
     * @param requested_extension The extension to check
     * @returns True if the loader knows how to load the extension and it is present in the glTF file, false if not
     */
    static bool is_extension_enabled(const std::string &requested_extension);
    
    /**
     * @brief Finds whether an extension exists inside a tinygltf extension map and returns the result
     * @param tinygltf_extensions The extension map associated with a given tinygltf object
     * @param extension The extension to check
     * @returns A pointer to the value of the extension object, nullptr if it isn't found
     */
    static tinygltf::Value *
    get_extension(tinygltf::ExtensionMap &tinygltf_extensions, const std::string &extension);
    
    Device const &device;
    
    tinygltf::Model model;
    
    std::string model_path;
    
    /// The extensions that the GLTFLoader can load mapped to whether they should be enabled or not
    static std::unordered_map<std::string, bool> supported_extensions;
    
private:
    //    sg::Scene load_scene(int scene_index = -1);
    
    std::unique_ptr<sg::SubMesh> load_model(uint32_t index);
};

}        // namespace vox
