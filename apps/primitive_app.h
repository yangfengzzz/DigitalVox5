//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef primitive_app_hpp
#define primitive_app_hpp

#include "forward_application.h"

namespace vox {
class PrimitiveApp : public ForwardApplication {
public:
    void load_scene() override;
};
}

#endif /* primitive_app_hpp */
