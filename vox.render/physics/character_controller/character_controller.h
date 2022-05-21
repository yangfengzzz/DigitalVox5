//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "../../component.h"
#include "../physics.h"
#include "point3.h"

namespace vox::physics {

class CharacterController : public Component {
public:
    explicit CharacterController(Entity *entity);

    PxControllerCollisionFlags Move(const Vector3F &disp, float min_dist, float elapsed_time);

    bool SetPosition(const Point3F &position);

    [[nodiscard]] Point3F Position() const;

    bool SetFootPosition(const Vector3F &position);

    [[nodiscard]] Vector3F FootPosition() const;

    void SetStepOffset(float offset);

    [[nodiscard]] float StepOffset() const;

    void SetNonWalkableMode(PxControllerNonWalkableMode::Enum flag);

    [[nodiscard]] PxControllerNonWalkableMode::Enum NonWalkableMode() const;

    [[nodiscard]] float ContactOffset() const;

    void SetContactOffset(float offset);

    [[nodiscard]] Vector3F UpDirection() const;

    void SetUpDirection(const Vector3F &up);

    [[nodiscard]] float SlopeLimit() const;

    void SetSlopeLimit(float slope_limit);

    void InvalidateCache();

    void State(PxControllerState &state) const;

    void Stats(PxControllerStats &stats) const;

    void Resize(float height);

private:
    friend class PhysicsManager;

    void OnLateUpdate();

    void OnEnable() override;

    void OnDisable() override;

protected:
    PxController *native_controller_{};
};

}  // namespace vox::physics