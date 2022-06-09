//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/singleton.h"

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

    static WireframeManager &GetSingleton();

    static WireframeManager *GetSingletonPtr();
};
template <>
inline WireframeManager *Singleton<WireframeManager>::ms_singleton = nullptr;

}  // namespace vox
