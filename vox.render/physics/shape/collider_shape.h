//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.math/transform3.h"
#include "vox.render/physics/physics.h"
#include "vox.render/scene_forward.h"

namespace vox::physics {
class ColliderShape {
public:
    ColliderShape();

    Collider *GetCollider();

public:
    void SetLocalPose(const Transform3F &pose);

    [[nodiscard]] Transform3F LocalPose() const;

    void SetPosition(const Vector3F &pos);

    [[nodiscard]] Vector3F Position() const;

    virtual void SetWorldScale(const Vector3F &scale);

public:
    void SetMaterial(PxMaterial *materials);

    PxMaterial *Material();

public:
    void SetQueryFilterData(const PxFilterData &data);

    PxFilterData QueryFilterData();

    uint32_t UniqueId();

public:
    void SetFlag(PxShapeFlag::Enum flag, bool value);

    void SetFlags(const PxShapeFlags &in_flags);

    [[nodiscard]] PxShapeFlags GetFlags() const;

    bool Trigger();

    void SetTrigger(bool is_trigger);

    bool SceneQuery();

    void SetSceneQuery(bool is_query);

public:
#ifdef DEBUG
    virtual void SetEntity(Entity *value);

    void RemoveEntity(Entity *value);

    Point3F GetLocalTranslation();
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