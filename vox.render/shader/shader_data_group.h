//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox {
/**
 * Shader data grouping.
 */
enum class ShaderDataGroup {
    /** Scene group. */
    SCENE,
    /** Camera group. */
    CAMERA,
    /** Renderer group. */
    RENDERER,
    /** material group. */
    MATERIAL,
    
    /** compute group. */
    COMPUTE
};

}