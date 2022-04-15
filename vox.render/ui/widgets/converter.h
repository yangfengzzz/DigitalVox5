//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_UI_WIDGETS_CONVERTER_H_
#define DIGITALVOX_VOX_RENDER_UI_WIDGETS_CONVERTER_H_

#include <imgui.h>
#include "vector2.h"
#include "vector4.h"
#include "color.h"

namespace vox::ui {
/**
 * Handles imgui conversion to/from arche types
 */
class Converter {
public:
    /**
     * Convert the given Color to ImVec4
     */
    static ImVec4 to_imVec4(const Color &p_value);
    
    /**
     * Convert the given ImVec4 to Color
     */
    static Color to_color(const ImVec4 &p_value);
    
    /**
     * Convert the given FVector2 to ImVec2
     */
    static ImVec2 to_imVec2(const Vector2F &p_value);
    
    /**
     * Convert the given ImVec2 to FVector2
     */
    static Vector2F to_vector2F(const ImVec2 &p_value);
};

}
#endif /* DIGITALVOX_VOX_RENDER_UI_WIDGETS_CONVERTER_H_ */
