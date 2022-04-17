//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "render_element.h"

namespace vox {
RenderElement::RenderElement(Renderer *renderer, MeshPtr mesh,
                             const SubMesh *sub_mesh, MaterialPtr material):
renderer(renderer),
mesh(mesh),
sub_mesh(sub_mesh),
material(material) {
}

}
