//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include <assimp/scene.h>
#include "scene_forward.h"
#include "core/device.h"
#include "image.h"

namespace vox {
class AssimpParser {
public:
    explicit AssimpParser(Device &device);
    
    /**
     * Simply load meshes from a file using assimp
     * Return true on success
     */
    void load_model(Entity *root, const std::string &file, unsigned int p_flags);
    
    /**
     * Processes a node in a recursive fashion.
     * Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
     */
    void process_node(Entity *root, aiNode *node, const aiScene *scene);
    
    void process_mesh(Entity *root, aiMesh *mesh, const aiScene *scene);
    
    std::shared_ptr<Material> process_material(aiMaterial *material);
    
    std::shared_ptr<Image> process_textures(aiMaterial *mat, aiTextureType type);
    
private:
    static std::string to_string(aiShadingMode mode);
    
    std::string directory_;
    Device &device_;
};

}
