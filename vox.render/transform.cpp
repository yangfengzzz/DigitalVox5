//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/transform.h"

#include "vox.math/math_utils.h"
#include "vox.math/matrix_utils.h"
#include "vox.render/entity.h"

namespace vox {
std::string Transform::name() { return "Transform"; }

Transform::Transform(Entity *entity) : Component(entity) {}

Point3F Transform::Position() { return position_; }

void Transform::SetPosition(const Point3F &value) {
    position_ = value;
    SetDirtyFlagTrue(TransformFlag::LOCAL_MATRIX);
    UpdateWorldPositionFlag();
}

Point3F Transform::WorldPosition() {
    if (IsContainDirtyFlag(TransformFlag::WORLD_POSITION)) {
        if (GetParentTransform()) {
            world_position_ = getTranslation(WorldMatrix());
        } else {
            world_position_ = position_;
        }
        SetDirtyFlagFalse(TransformFlag::WORLD_POSITION);
    }
    return world_position_;
}

void Transform::SetWorldPosition(const Point3F &value) {
    world_position_ = value;
    const auto kParent = GetParentTransform();
    if (kParent) {
        position_ = kParent->WorldMatrix().inverse() * value;
    } else {
        position_ = value;
    }
    SetPosition(position_);
    SetDirtyFlagFalse(TransformFlag::WORLD_POSITION);
}

Vector3F Transform::Rotation() {
    if (IsContainDirtyFlag(TransformFlag::LOCAL_EULER)) {
        rotation_ = rotation_quaternion_.toEuler();
        rotation_ *= kRadianToDegree;  // radians to degrees

        SetDirtyFlagFalse(TransformFlag::LOCAL_EULER);
    }
    return rotation_;
}

void Transform::SetRotation(const Vector3F &value) {
    rotation_ = value;
    SetDirtyFlagTrue(TransformFlag::LOCAL_MATRIX | TransformFlag::LOCAL_QUAT);
    SetDirtyFlagFalse(TransformFlag::LOCAL_EULER);
    UpdateWorldRotationFlag();
}

Vector3F Transform::WorldRotation() {
    if (IsContainDirtyFlag(TransformFlag::WORLD_EULER)) {
        world_rotation_ = WorldRotationQuaternion().toEuler();
        world_rotation_ *= kRadianToDegree;  // Radian to angle
        SetDirtyFlagFalse(TransformFlag::WORLD_EULER);
    }
    return world_rotation_;
}

void Transform::SetWorldRotation(const Vector3F &value) {
    world_rotation_ = value;
    world_rotation_quaternion_ = QuaternionF::makeRotationEuler(degreesToRadians(value.x), degreesToRadians(value.y),
                                                                degreesToRadians(value.z));
    SetWorldRotationQuaternion(world_rotation_quaternion_);
    SetDirtyFlagFalse(TransformFlag::WORLD_EULER);
}

QuaternionF Transform::RotationQuaternion() {
    if (IsContainDirtyFlag(TransformFlag::LOCAL_QUAT)) {
        rotation_quaternion_ = QuaternionF::makeRotationEuler(
                degreesToRadians(rotation_.x), degreesToRadians(rotation_.y), degreesToRadians(rotation_.z));
        SetDirtyFlagFalse(TransformFlag::LOCAL_QUAT);
    }
    return rotation_quaternion_;
}

void Transform::SetRotationQuaternion(const QuaternionF &value) {
    rotation_quaternion_ = value.normalized();
    SetDirtyFlagTrue(TransformFlag::LOCAL_MATRIX | TransformFlag::LOCAL_EULER);
    SetDirtyFlagFalse(TransformFlag::LOCAL_QUAT);
    UpdateWorldRotationFlag();
}

QuaternionF Transform::WorldRotationQuaternion() {
    if (IsContainDirtyFlag(TransformFlag::WORLD_QUAT)) {
        const auto kParent = GetParentTransform();
        if (kParent) {
            world_rotation_quaternion_ = kParent->WorldRotationQuaternion() * RotationQuaternion();
        } else {
            world_rotation_quaternion_ = RotationQuaternion();
        }
        SetDirtyFlagFalse(TransformFlag::WORLD_QUAT);
    }
    return world_rotation_quaternion_;
}

void Transform::SetWorldRotationQuaternion(const QuaternionF &value) {
    world_rotation_quaternion_ = value.normalized();
    const auto kParent = GetParentTransform();
    if (kParent) {
        auto temp_quat_0 = kParent->WorldRotationQuaternion().inverse();
        rotation_quaternion_ = world_rotation_quaternion_ * temp_quat_0;
    } else {
        rotation_quaternion_ = world_rotation_quaternion_;
    }
    SetRotationQuaternion(rotation_quaternion_);
    SetDirtyFlagFalse(TransformFlag::WORLD_QUAT);
}

Vector3F Transform::Scale() { return scale_; }

void Transform::SetScale(const Vector3F &value) {
    scale_ = value;
    SetDirtyFlagTrue(TransformFlag::LOCAL_MATRIX);
    UpdateWorldScaleFlag();
}

Vector3F Transform::LossyWorldScale() {
    if (IsContainDirtyFlag(TransformFlag::WORLD_SCALE)) {
        if (GetParentTransform()) {
            const auto kScaleMat = GetScaleMatrix();
            lossy_world_scale_ = Vector3F(kScaleMat[0], kScaleMat[4], kScaleMat[8]);
        } else {
            lossy_world_scale_ = scale_;
        }
        SetDirtyFlagFalse(TransformFlag::WORLD_SCALE);
    }
    return lossy_world_scale_;
}

Matrix4x4F Transform::LocalMatrix() {
    if (IsContainDirtyFlag(TransformFlag::LOCAL_MATRIX)) {
        local_matrix_ = makeAffineMatrix(scale_, RotationQuaternion(), position_);
        SetDirtyFlagFalse(TransformFlag::LOCAL_MATRIX);
    }
    return local_matrix_;
}

void Transform::SetLocalMatrix(const Matrix4x4F &value) {
    local_matrix_ = value;
    decompose(local_matrix_, position_, rotation_quaternion_, scale_);
    SetDirtyFlagTrue(TransformFlag::LOCAL_EULER);
    SetDirtyFlagFalse(TransformFlag::LOCAL_MATRIX);
    UpdateAllWorldFlag();
}

Matrix4x4F Transform::WorldMatrix() {
    if (IsContainDirtyFlag(TransformFlag::WORLD_MATRIX)) {
        const auto kParent = GetParentTransform();
        if (kParent) {
            world_matrix_ = kParent->WorldMatrix() * LocalMatrix();
        } else {
            world_matrix_ = LocalMatrix();
        }
        SetDirtyFlagFalse(TransformFlag::WORLD_MATRIX);
    }
    return world_matrix_;
}

void Transform::SetWorldMatrix(const Matrix4x4F &value) {
    world_matrix_ = value;
    const auto kParent = GetParentTransform();
    if (kParent) {
        auto temp_mat_42 = kParent->WorldMatrix().inverse();
        local_matrix_ = value * temp_mat_42;
    } else {
        local_matrix_ = value;
    }
    SetLocalMatrix(local_matrix_);
    SetDirtyFlagFalse(TransformFlag::WORLD_MATRIX);
}

void Transform::SetPosition(float x, float y, float z) {
    position_ = Point3F(x, y, z);
    SetPosition(position_);
}

void Transform::SetRotation(float x, float y, float z) {
    rotation_ = Vector3F(x, y, z);
    SetRotation(rotation_);
}

void Transform::SetRotationQuaternion(float x, float y, float z, float w) {
    rotation_quaternion_ = QuaternionF(x, y, z, w);
    SetRotationQuaternion(rotation_quaternion_);
}

void Transform::SetScale(float x, float y, float z) {
    scale_ = Vector3F(x, y, z);
    SetScale(scale_);
}

void Transform::SetWorldPosition(float x, float y, float z) {
    world_position_ = Point3F(x, y, z);
    SetWorldPosition(world_position_);
}

void Transform::SetWorldRotation(float x, float y, float z) {
    world_rotation_ = Vector3F(x, y, z);
    SetWorldRotation(world_rotation_);
}

void Transform::SetWorldRotationQuaternion(float x, float y, float z, float w) {
    world_rotation_quaternion_ = QuaternionF(x, y, z, w);
    SetWorldRotationQuaternion(world_rotation_quaternion_);
}

Vector3F Transform::WorldForward() {
    const auto &e = WorldMatrix();
    auto forward = Vector3F(-e[8], -e[9], -e[10]);
    return forward.normalized();
}

Vector3F Transform::WorldRight() {
    const auto &e = WorldMatrix();
    auto right = Vector3F(e[0], e[1], e[2]);
    return right.normalized();
}

Vector3F Transform::WorldUp() {
    const auto &e = WorldMatrix();
    auto up = Vector3F(e[4], e[5], e[6]);
    return up.normalized();
}

void Transform::Translate(const Vector3F &translation, bool relative_to_local) {
    if (relative_to_local) {
        position_ = position_ + translation;
        SetPosition(position_);
    } else {
        world_position_ = world_position_ + translation;
        SetWorldPosition(world_position_);
    }
}

void Transform::Translate(float x, float y, float z, bool relative_to_local) {
    Translate(Vector3F(x, y, z), relative_to_local);
}

void Transform::Rotate(const Vector3F &rotation, bool relative_to_local) {
    RotateXyz(rotation.x, rotation.y, rotation.z, relative_to_local);
}

void Transform::Rotate(float x, float y, float z, bool relative_to_local) { RotateXyz(x, y, z, relative_to_local); }

void Transform::RotateByAxis(const Vector3F &axis, float angle, bool relative_to_local) {
    const auto kRad = angle * kDegreeToRadian;
    QuaternionF temp_quat_0(axis, kRad);
    RotateByQuat(temp_quat_0, relative_to_local);
}

void Transform::LookAt(const Point3F &world_position, const Vector3F &world_up) {
    const auto kPosition = this->WorldPosition();
    if (std::abs(kPosition.x - world_position.x) < std::numeric_limits<float>::epsilon() &&
        std::abs(kPosition.y - world_position.y) < std::numeric_limits<float>::epsilon() &&
        std::abs(kPosition.z - world_position.z) < std::numeric_limits<float>::epsilon()) {
        return;
    }
    Matrix4x4F rot_mat = makeLookAtMatrix(kPosition, world_position, world_up);
    auto world_rotation_quaternion = getRotation(rot_mat);
    world_rotation_quaternion = world_rotation_quaternion.inverse();
    SetWorldRotationQuaternion(world_rotation_quaternion);
}

std::unique_ptr<UpdateFlag> Transform::RegisterWorldChangeFlag() { return update_flag_manager_.Registration(); }

void Transform::ParentChange() {
    is_parent_dirty_ = true;
    UpdateAllWorldFlag();
}

void Transform::UpdateWorldPositionFlag() {
    if (!IsContainDirtyFlags(TransformFlag::WM_WP)) {
        WorldAssociatedChange(TransformFlag::WM_WP);
        const auto &node_children = entity_->children;
        for (const auto &i : node_children) {
            i->transform->UpdateWorldPositionFlag();
        }
    }
}

void Transform::UpdateWorldRotationFlag() {
    if (!IsContainDirtyFlags(TransformFlag::WM_WE_WQ)) {
        WorldAssociatedChange(TransformFlag::WM_WE_WQ);
        const auto &node_children = entity_->children;
        for (const auto &i : node_children) {
            // Rotation update of parent entity will trigger world position and rotation update of all child entity.
            i->transform->UpdateWorldPositionAndRotationFlag();
        }
    }
}

void Transform::UpdateWorldPositionAndRotationFlag() {
    if (!IsContainDirtyFlags(TransformFlag::WM_WP_WE_WQ)) {
        WorldAssociatedChange(TransformFlag::WM_WP_WE_WQ);
        const auto &node_children = entity_->children;
        for (const auto &i : node_children) {
            i->transform->UpdateWorldPositionAndRotationFlag();
        }
    }
}

void Transform::UpdateWorldScaleFlag() {
    if (!IsContainDirtyFlags(TransformFlag::WM_WS)) {
        WorldAssociatedChange(TransformFlag::WM_WS);
        const auto &node_children = entity_->children;
        for (const auto &i : node_children) {
            i->transform->UpdateWorldPositionAndScaleFlag();
        }
    }
}

void Transform::UpdateWorldPositionAndScaleFlag() {
    if (!IsContainDirtyFlags(TransformFlag::WM_WP_WS)) {
        WorldAssociatedChange(TransformFlag::WM_WP_WS);
        const auto &node_children = entity_->children;
        for (const auto &i : node_children) {
            i->transform->UpdateWorldPositionAndScaleFlag();
        }
    }
}

void Transform::UpdateAllWorldFlag() {
    if (!IsContainDirtyFlags(TransformFlag::WM_WP_WE_WQ_WS)) {
        WorldAssociatedChange(TransformFlag::WM_WP_WE_WQ_WS);
        const auto &node_children = entity_->children;
        for (const auto &i : node_children) {
            i->transform->UpdateAllWorldFlag();
        }
    }
}

Transform *Transform::GetParentTransform() {
    if (!is_parent_dirty_) {
        return parent_transform_cache_;
    }
    Transform *parent_cache = nullptr;
    auto parent = entity_->Parent();
    while (parent) {
        const auto &transform = parent->transform;
        if (transform) {
            parent_cache = transform;
            break;
        } else {
            parent = parent->Parent();
        }
    }
    parent_transform_cache_ = parent_cache;
    is_parent_dirty_ = false;
    return parent_cache;
}

Matrix3x3F Transform::GetScaleMatrix() {
    Matrix3x3F world_rot_sca_mat = WorldMatrix().matrix3();
    Quaternion inv_rotation = WorldRotationQuaternion().inverse();
    Matrix3x3F inv_rotation_mat = inv_rotation.matrix3();
    return inv_rotation_mat * world_rot_sca_mat;
}

bool Transform::IsContainDirtyFlags(int target_dirty_flags) const {
    return (dirty_flag_ & target_dirty_flags) == target_dirty_flags;
}

bool Transform::IsContainDirtyFlag(int type) const { return (dirty_flag_ & type) != 0; }

void Transform::SetDirtyFlagTrue(int type) { dirty_flag_ |= type; }

void Transform::SetDirtyFlagFalse(int type) { dirty_flag_ &= ~type; }

void Transform::WorldAssociatedChange(int type) {
    dirty_flag_ |= type;
    update_flag_manager_.Distribute();
}

void Transform::RotateByQuat(const QuaternionF &rotate_quat, bool relative_to_local) {
    if (relative_to_local) {
        rotation_quaternion_ = RotationQuaternion() * rotate_quat;
        SetRotationQuaternion(rotation_quaternion_);
    } else {
        world_rotation_quaternion_ = WorldRotationQuaternion() * rotate_quat;
        SetWorldRotationQuaternion(world_rotation_quaternion_);
    }
}

void Transform::RotateXyz(float x, float y, float z, bool relative_to_local) {
    const auto kRotQuat = QuaternionF::makeRotationEuler(x * kDegreeToRadian, y * kDegreeToRadian, z * kDegreeToRadian);
    RotateByQuat(kRotQuat, relative_to_local);
}

// MARK: - Reflection
void Transform::OnSerialize(nlohmann::json &data) {}

void Transform::OnDeserialize(const nlohmann::json &data) {}

void Transform::OnInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox
