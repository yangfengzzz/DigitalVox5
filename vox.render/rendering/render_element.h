//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef render_element_hpp
#define render_element_hpp

#include "scene_forward.h"

namespace vox {
/**
 * Render element.
 */
struct RenderElement {
    /** Render component. */
    Renderer *renderer;
    /** Mesh. */
    MeshPtr mesh;
    /** Sub mesh. */
    const SubMesh *sub_mesh;
    /** Material. */
    MaterialPtr material;
    
    RenderElement(Renderer *renderer, MeshPtr mesh, const SubMesh *sub_mesh, MaterialPtr material);
};

}

#endif /* render_element_hpp */
