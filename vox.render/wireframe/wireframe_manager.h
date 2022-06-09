//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.base/singleton.h"
#include "vox.math/matrix4x4.h"
#include "vox.math/vector3.h"
#include "vox.render/mesh/index_buffer_binding.h"
#include "vox.render/mesh/mesh_renderer.h"

namespace vox {
class WireframeManager : public Singleton<WireframeManager> {
public:
    /**
     \brief Default color values used for wireframe rendering.
     */
    struct FrameColor {
        enum Enum {
            RGB_BLACK = 0xff000000,
            RGB_RED = 0xffff0000,
            RGB_GREEN = 0xff00ff00,
            RGB_BLUE = 0xff0000ff,
            RGB_YELLOW = 0xffffff00,
            RGB_MAGENTA = 0xffff00ff,
            RGB_CYAN = 0xff00ffff,
            RGB_WHITE = 0xffffffff,
            RGB_GREY = 0xff808080,
            RGB_DARKRED = 0x88880000,
            RGB_DARKGREEN = 0x88008800,
            RGB_DARKBLUE = 0x88000088
        };
    };

    struct RenderDebugVertex {
        Vector3F pos;
        uint32_t color;
    };

    struct RenderDebugBuffer {
        std::vector<RenderDebugVertex> vertex{};
        std::vector<uint32_t> indices{};
        std::unique_ptr<core::Buffer> vertex_buffer{nullptr};
        std::unique_ptr<vox::IndexBufferBinding> indices_buffer{nullptr};
        MeshRenderer *renderer{nullptr};

        void Clear();
    };

    static WireframeManager &GetSingleton();

    static WireframeManager *GetSingletonPtr();

    explicit WireframeManager(Entity *entity);

    void Clear();

    void Flush();

public:
    void AddLine(const Vector3F &a, const Vector3F &b, uint32_t color);

    void AddVector(const Vector3F &start, const Vector3F &vec, uint32_t color) { AddLine(start, start + vec, color); }

    void AddLine(const Matrix4x4F &t, const Vector3F &a, const Vector3F &b, uint32_t color);

    void AddVector(const Matrix4x4F &t, const Vector3F &start, const Vector3F &vec, uint32_t color) {
        AddLine(t, start, start + vec, color);
    }

private:
    Entity *entity_{nullptr};

    MaterialPtr material_{nullptr};
    RenderDebugBuffer points_{};
    RenderDebugBuffer lines_{};
    RenderDebugBuffer triangles_{};
    VertexInputState vertex_input_state_;
};
template <>
inline WireframeManager *Singleton<WireframeManager>::ms_singleton = nullptr;

}  // namespace vox
