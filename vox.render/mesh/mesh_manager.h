//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "mesh.h"
#include "singleton.h"

namespace vox {
class MeshManager : public Singleton<MeshManager> {
public:
    static MeshManager &get_singleton();
    
    static MeshManager *get_singleton_ptr();
    
    /**
     * @brief Loads mesh
     */
    std::shared_ptr<Mesh> load_mesh(const std::string &file);
    
    void collect_garbage();
    
private:
    std::unordered_map<std::string, std::shared_ptr<Mesh>> mesh_pool_;
};

template<> inline MeshManager *Singleton<MeshManager>::ms_singleton_{nullptr};
}
