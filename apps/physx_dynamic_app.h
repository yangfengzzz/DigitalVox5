//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <random>

#include "vox.render/forward_application.h"

namespace vox {
class PhysXDynamicApp : public ForwardApplication {
public:
    void LoadScene() override;

    void InputEvent(const vox::InputEvent &input_event) override;

private:
    Entity *AddSphere(float radius, const Point3F &position, const QuaternionF &rotation, const Vector3F &velocity);

    Entity *AddCapsule(float radius, float height, const Point3F &position, const QuaternionF &rotation);

private:
    Entity *root_entity_{nullptr};
    std::default_random_engine e;
    std::uniform_real_distribution<float> u;
};

}  // namespace vox
