//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_PHYSICS_CHARACTER_CONTROLLER_CHARACTER_CONTROLLER_H_
#define DIGITALVOX_VOX_RENDER_PHYSICS_CHARACTER_CONTROLLER_CHARACTER_CONTROLLER_H_

#include "../../component.h"
#include "../physics.h"
#include "point3.h"

namespace vox::physics {

class CharacterController : public Component {
public:
    explicit CharacterController(Entity *entity);

    PxControllerCollisionFlags move(const Vector3F &disp, float min_dist, float elapsed_time);

    bool set_position(const Point3F &position);

    [[nodiscard]] Point3F position() const;

    bool set_foot_position(const Vector3F &position);

    [[nodiscard]] Vector3F foot_position() const;

    void set_step_offset(float offset);

    [[nodiscard]] float step_offset() const;

    void set_non_walkable_mode(PxControllerNonWalkableMode::Enum flag);

    [[nodiscard]] PxControllerNonWalkableMode::Enum non_walkable_mode() const;

    [[nodiscard]] float contact_offset() const;

    void set_contact_offset(float offset);

    [[nodiscard]] Vector3F up_direction() const;

    void set_up_direction(const Vector3F &up);

    [[nodiscard]] float slope_limit() const;

    void set_slope_limit(float slope_limit);

    void invalidate_cache();

    void state(PxControllerState &state) const;

    void stats(PxControllerStats &stats) const;

    void resize(float height);

private:
    friend class PhysicsManager;

    void on_late_update();

    void OnEnable() override;

    void OnDisable() override;

protected:
    PxController *native_controller_{};
};

}  // namespace vox::physics

#endif /* DIGITALVOX_VOX_RENDER_PHYSICS_CHARACTER_CONTROLLER_CHARACTER_CONTROLLER_H_ */
