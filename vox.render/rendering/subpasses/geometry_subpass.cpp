//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "geometry_subpass.h"
#include "material/material.h"
#include "mesh/mesh.h"
#include "camera.h"
#include "renderer.h"
#include "components_manager.h"

namespace vox {
GeometrySubpass::GeometrySubpass(RenderContext &render_context, ShaderSource &&vertex_source,
                                 ShaderSource &&fragment_source, Scene *scene, Camera *camera):
Subpass{render_context, std::move(vertex_source), std::move(fragment_source)},
camera{camera},
scene{scene} {
}

void GeometrySubpass::set_thread_index(uint32_t index) {
    thread_index = index;
}

void GeometrySubpass::draw(CommandBuffer &command_buffer) {
    auto compileMacros = ShaderVariant();
    
    std::vector<RenderElement> opaqueQueue;
    std::vector<RenderElement> alphaTestQueue;
    std::vector<RenderElement> transparentQueue;
    ComponentsManager::get_singleton().call_render(camera, opaqueQueue, alphaTestQueue, transparentQueue);
    std::sort(opaqueQueue.begin(), opaqueQueue.end(), _compareFromNearToFar);
    std::sort(alphaTestQueue.begin(), alphaTestQueue.end(), _compareFromNearToFar);
    std::sort(transparentQueue.begin(), transparentQueue.end(), _compareFromFarToNear);
    
    _drawElement(command_buffer, opaqueQueue, compileMacros);
    _drawElement(command_buffer, alphaTestQueue, compileMacros);
    _drawElement(command_buffer, transparentQueue, compileMacros);
}

void GeometrySubpass::_drawElement(CommandBuffer &command_buffer,
                                   const std::vector<RenderElement> &items,
                                   const ShaderVariant& variant) {
    for (auto &element: items) {
        auto &device = command_buffer.get_device();

        auto &renderer = element.renderer;
        auto &material = element.material;
        auto &sub_mesh = element.sub_mesh;
        auto &mesh = element.mesh;
        ScopedDebugLabel submesh_debug_label{command_buffer, mesh->name_.c_str()};
        
        command_buffer.set_rasterization_state(material->rasterization_state_);
        command_buffer.set_multisample_state(material->multisample_state_);
    }
}

}
