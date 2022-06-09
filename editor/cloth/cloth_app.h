//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.cloth/cloth_controller.h"
#include "vox.editor/demo_application.h"

namespace vox::editor {
class ClothApp : public DemoApplication {
public:
    void SetupUi() override;

    Camera *LoadScene(Entity *root_entity) override;

    void InputEvent(const vox::InputEvent &input_event) override;

    void Update(float delta_time) override;

private:
    cloth::ClothController controller_;
    Camera *scene_camera_{nullptr};

    void InitializeCloth(Entity *entity, const physx::PxVec3 &offset);
    nv::cloth::Fabric *fabric_{nullptr};
    nv::cloth::Solver *solver_{nullptr};
    cloth::ClothRenderer *cloth_actor_{nullptr};
};

}  // namespace vox::editor
