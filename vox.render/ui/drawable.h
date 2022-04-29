//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

namespace vox::ui {
/**
 * Interface for any drawable class
 */
class Drawable {
public:
    virtual void draw() = 0;
    
protected:
    virtual ~Drawable() = default;
};

}