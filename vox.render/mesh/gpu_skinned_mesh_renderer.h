//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/mesh/mesh_renderer.h"

namespace vox {

class GpuSkinnedMeshRenderer : public MeshRenderer {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    struct Skin {
        std::string name;
        std::vector<Matrix4x4F> inverse_bind_matrices;
        std::vector<Entity *> joints;
    };
    using SkinPtr = std::shared_ptr<Skin>;

public:
    explicit GpuSkinnedMeshRenderer(Entity *entity);

    /**
     * Skin Object.
     */
    SkinPtr Skin();

    void SetSkin(const SkinPtr &skin);

    void Update(float delta_time) override;

public:
    /**
     * Called when the serialization is asked
     */
    void OnSerialize(nlohmann::json &data) override;

    /**
     * Called when the deserialization is asked
     */
    void OnDeserialize(const nlohmann::json &data) override;

    /**
     * Defines how the component should be drawn in the inspector
     */
    void OnInspector(ui::WidgetContainer &p_root) override;

private:
    SkinPtr skin_;

    void InitJoints();

    bool has_init_joints_ = false;

    std::vector<float> joint_matrix_{};
    const std::string joint_matrix_property_;
};

}  // namespace vox
