//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_PHYSICS_SHAPE_COLLIDER_SHAPE_H_
#define DIGITALVOX_VOX_RENDER_PHYSICS_SHAPE_COLLIDER_SHAPE_H_

#include <vector>

#include "../physics.h"
#include "scene_forward.h"
#include "transform3.h"

namespace vox::physics {
class ColliderShape {
public:
    ColliderShape();

    Collider *collider();

public:
    void set_local_pose(const Transform3F &pose);

    [[nodiscard]] Transform3F local_pose() const;

    void set_position(const Vector3F &pos);

    [[nodiscard]] Vector3F position() const;

    virtual void set_world_scale(const Vector3F &scale);

public:
    void set_material(PxMaterial *materials);

    PxMaterial *material();

public:
    void set_query_filter_data(const PxFilterData &data);

    PxFilterData query_filter_data();

    uint32_t unique_id();

public:
    void set_flag(PxShapeFlag::Enum flag, bool value);

    void set_flags(const PxShapeFlags &in_flags);

    [[nodiscard]] PxShapeFlags get_flags() const;

    bool trigger();

    void set_trigger(bool is_trigger);

    bool scene_query();

    void set_scene_query(bool is_query);

public:
#ifdef DEBUG
    virtual void set_entity(Entity *value);

    void remove_entity(Entity *value);

    Point3F get_local_translation();
#endif

protected:
    friend class Collider;

    PxShape *native_shape_ = nullptr;
    std::shared_ptr<PxGeometry> native_geometry_ = nullptr;
    PxMaterial *native_material_ = nullptr;

    Collider *collider_ = nullptr;

    Vector3F scale_ = Vector3F(1, 1, 1);
    Transform3F pose_;
    static constexpr float half_sqrt_ = 0.70710678118655;

#ifdef DEBUG
    Entity *entity_{nullptr};
#endif
};

}  // namespace vox::physics
#endif /* DIGITALVOX_VOX_RENDER_PHYSICS_SHAPE_COLLIDER_SHAPE_H_ */
