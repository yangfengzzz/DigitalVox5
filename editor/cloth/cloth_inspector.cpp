//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "editor/cloth/cloth_inspector.h"

#include "vox.cloth/NvCloth/Factory.h"

namespace vox::editor::ui {
namespace {
class ClothUI : public vox::ui::Widget {
public:
    explicit ClothUI(cloth::ClothController &controller) : controller_(controller) {}

    void DrawImpl() override {
        UpdateClothUi();
        UpdateSolverUi();
        UpdateDebugUi();

        DrawDebugVisualization();
    }

private:
    cloth::ClothController &controller_;

    void UpdateClothUi() {
        static int active_cloth = 0;

        if (ImGui::TreeNode("Cloth Properties")) {
            active_cloth = std::min(active_cloth, (int)controller_.ClothList().size() - 1);
            for (int i = 0; i < (int)controller_.ClothList().size(); i++) {
                if (i) ImGui::SameLine();
                std::stringstream cloth_name;
                cloth_name << "Cloth " << i;
                ImGui::RadioButton(cloth_name.str().c_str(), &active_cloth, i);
            }

            nv::cloth::Cloth *cloth = controller_.ClothList()[active_cloth]->cloth_;
            {
                bool b = cloth->isContinuousCollisionEnabled();
                if (ImGui::Checkbox("Continuous Collision Detection (CCD)", &b)) cloth->enableContinuousCollision(b);
            }
            {
                physx::PxVec3 f_3 = cloth->getDamping();
                if (ImGui::DragFloat3("Damping", &f_3.x, 0.02f, 0.0f, 1.0f, "%.2f")) cloth->setDamping(f_3);
                float f = f_3.maxElement();
                if (ImGui::DragFloat("Damping xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setDamping(physx::PxVec3(f, f, f));
            }
            {
                float f = cloth->getDragCoefficient();
                if (ImGui::DragFloat("Drag Coefficient", &f, 0.02f, 0.0f, 0.99f, "%.2f")) cloth->setDragCoefficient(f);
            }
            {
                float f = cloth->getFriction();
                if (ImGui::DragFloat("Friction", &f, 0.04f, 0.0f, 2.0f, "%.2f")) cloth->setFriction(f);
            }
            {
                physx::PxVec3 f_3 = cloth->getGravity();
                if (ImGui::DragFloat3("Gravity", &f_3.x, 0.5f, -50.0f, 50.0f, "%.1f")) cloth->setGravity(f_3);
            }
            {
                float f = cloth->getLiftCoefficient();
                if (ImGui::DragFloat("Lift Coefficient", &f, 0.02f, 0.0f, 1.0f, "%.2f")) cloth->setLiftCoefficient(f);
            }
            {
                physx::PxVec3 f_3 = cloth->getLinearInertia();
                if (ImGui::DragFloat3("Linear Inertia", &f_3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setLinearInertia(f_3);
                float f = f_3.maxElement();
                if (ImGui::DragFloat("Linear Inertia xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setLinearInertia(physx::PxVec3(f, f, f));
            }
            {
                physx::PxVec3 f_3 = cloth->getAngularInertia();
                if (ImGui::DragFloat3("Angular Inertia", &f_3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setAngularInertia(f_3);
                float f = f_3.maxElement();
                if (ImGui::DragFloat("Angular Inertia xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setAngularInertia(physx::PxVec3(f, f, f));
            }
            {
                physx::PxVec3 f_3 = cloth->getCentrifugalInertia();
                if (ImGui::DragFloat3("Centrifugal Inertia", &f_3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setCentrifugalInertia(f_3);
                float f = f_3.maxElement();
                if (ImGui::DragFloat("Centrifugal Inertia xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setCentrifugalInertia(physx::PxVec3(f, f, f));
            }
            {
                physx::PxVec3 f_3 = cloth->getLinearDrag();
                if (ImGui::DragFloat3("Linear Drag", &f_3.x, 0.02f, 0.0f, 1.0f, "%.2f")) cloth->setLinearDrag(f_3);
                float f = f_3.maxElement();
                if (ImGui::DragFloat("Linear Drag xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setLinearDrag(physx::PxVec3(f, f, f));
            }
            {
                physx::PxVec3 f_3 = cloth->getAngularDrag();
                if (ImGui::DragFloat3("Angular Drag", &f_3.x, 0.02f, 0.0f, 1.0f, "%.2f")) cloth->setAngularDrag(f_3);
                float f = f_3.maxElement();
                if (ImGui::DragFloat("Angular Drag xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setAngularDrag(physx::PxVec3(f, f, f));
            }
            {
                float f = cloth->getMotionConstraintScale();
                if (ImGui::DragFloat("Motion Constraint Scale", &f, 0.08f, 0.0f, 4.0f, "%.2f"))
                    cloth->setMotionConstraintScaleBias(f, cloth->getMotionConstraintBias());
            }
            {
                float f = cloth->getMotionConstraintBias();
                if (ImGui::DragFloat("Motion Constraint Bias", &f, 0.16f, 0.0f, 8.0f, "%.2f"))
                    cloth->setMotionConstraintScaleBias(cloth->getMotionConstraintScale(), f);
            }
            {
                float f = cloth->getSelfCollisionDistance();
                if (ImGui::DragFloat("Self Collision Distance", &f, 0.005f, 0.0f, 0.3f, "%.3f"))
                    cloth->setSelfCollisionDistance(f);
            }
            {
                float f = cloth->getSelfCollisionStiffness();
                if (ImGui::DragFloat("Self Collision Stiffness", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setSelfCollisionStiffness(f);
            }
            {
                float f = cloth->getSleepThreshold();
                if (ImGui::DragFloat("Sleep Threshold", &f, 0.02f, 0.0f, 1.0f, "%.2f")) cloth->setSleepThreshold(f);
            }
            {
                float f = cloth->getStiffnessFrequency();
                if (ImGui::DragFloat("Stiffness Frequency", &f, 1.0f, 0.0f, 600.0f, "%.0f",
                                     ImGuiSliderFlags_Logarithmic))
                    cloth->setStiffnessFrequency(f);
            }
            {
                float f = cloth->getSolverFrequency();
                if (ImGui::DragFloat("Solver Frequency", &f, 1.0f, 0.0f, 600.0f, "%.0f", ImGuiSliderFlags_Logarithmic))
                    cloth->setSolverFrequency(f);
            }
            {
                float f = cloth->getTetherConstraintScale();
                if (ImGui::DragFloat("Tether Constraint Scale", &f, 0.08f, 0.0f, 4.0f, "%.2f"))
                    cloth->setTetherConstraintScale(f);
            }
            {
                float f = cloth->getTetherConstraintStiffness();
                if (ImGui::DragFloat("Tether Constraint Stiffness", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setTetherConstraintStiffness(f);
            }
            {
                physx::PxVec3 f_3 = cloth->getWindVelocity();
                if (ImGui::DragFloat3("Wind Velocity", &f_3.x, 0.5f, -50.0f, 50.0f, "%.1f"))
                    cloth->setWindVelocity(f_3);
            }
            ImGui::TreePop();
        }
    }

    void UpdateSolverUi() {
        static int active_solver = 0;

        if (ImGui::TreeNode("Solver Properties")) {
            active_solver = std::min(active_solver, (int)controller_.SolverList().size() - 1);
            for (int i = 0; i < (int)controller_.SolverList().size(); i++) {
                if (i) ImGui::SameLine();
                std::stringstream cloth_name;
                cloth_name << "Solver " << i;
                ImGui::RadioButton(cloth_name.str().c_str(), &active_solver, i);
            }

            nv::cloth::Solver *solver = controller_.SolverList()[active_solver];

            {
                float f = solver->getInterCollisionDistance();
                if (ImGui::DragFloat("Inter Collision Distance", &f, 0.005f, 0.0f, 2.0f, "%.2f"))
                    solver->setInterCollisionDistance(f);
            }
            {
                uint32_t i = solver->getInterCollisionNbIterations();
                if (ImGui::DragInt("Inter Collision Iterations", (int *)&i, 0.25, 0, 16))
                    solver->setInterCollisionNbIterations(i);
            }
            {
                float f = solver->getInterCollisionStiffness();
                if (ImGui::DragFloat("Inter Collision Stiffness", &f, 0.005f, 0.0f, 1.0f, "%.2f"))
                    solver->setInterCollisionStiffness(f);
            }
            ImGui::TreePop();
        }
    }

    static void UpdateDebugUi() {
        if (ImGui::TreeNode("Debug Visualization")) {
            auto old = debug_visualization_flags_;
            ImGui::CheckboxFlags("Tethers (T)", &debug_visualization_flags_, DEBUG_VIS_TETHERS);
            ImGui::CheckboxFlags("Constraints (C)", &debug_visualization_flags_, DEBUG_VIS_CONSTRAINTS);
            if (debug_visualization_flags_ & DEBUG_VIS_CONSTRAINTS) {
                ImGui::DragInt("Start Constraint Phase Range", &scene_debug_render_params_.visible_phase_range_begin,
                               0.05, 0, 30);
                ImGui::DragInt("End", &scene_debug_render_params_.visible_phase_range_end, 0.05, 0, 30);
            }
            ImGui::CheckboxFlags("Constraint Stiffness (F)", &debug_visualization_flags_,
                                 DEBUG_VIS_CONSTRAINTS_STIFFNESS);
            ImGui::CheckboxFlags("Constraint Error (R)", &debug_visualization_flags_, DEBUG_VIS_CONSTRAINT_ERROR);
            ImGui::CheckboxFlags("Position Delta (L)", &debug_visualization_flags_, DEBUG_VIS_POSITION_DELTA);
            ImGui::CheckboxFlags("Bounding Box (X)", &debug_visualization_flags_, DEBUG_VIS_BOUNDING_BOX);
            ImGui::CheckboxFlags("Distance Constraints (Z)", &debug_visualization_flags_,
                                 DEBUG_VIS_DISTANCE_CONSTRAINTS);
            ImGui::TreePop();

            if (old != debug_visualization_flags_) debug_visualization_update_requested_ = true;
        }
    }

    void DrawDebugVisualization() {
        if (debug_visualization_flags_ & DEBUG_VIS_TETHERS) DebugRenderTethers();
        if (debug_visualization_flags_ & DEBUG_VIS_CONSTRAINTS) DebugRenderConstraints();
        if (debug_visualization_flags_ & DEBUG_VIS_CONSTRAINTS_STIFFNESS) DebugRenderConstraintStiffness();

        if (debug_visualization_flags_ & DEBUG_VIS_CONSTRAINT_ERROR) DebugRenderConstraintError();
        if (debug_visualization_flags_ & DEBUG_VIS_POSITION_DELTA) DebugRenderPositionDelta();
        if (debug_visualization_flags_ & DEBUG_VIS_BOUNDING_BOX) DebugRenderBoundingBox();
        if (debug_visualization_flags_ & DEBUG_VIS_DISTANCE_CONSTRAINTS) DebugRenderDistanceConstraints();

        debug_visualization_update_requested_ = false;
    }

    void DebugRenderDistanceConstraints() {}
    void DebugRenderTethers() {}
    void DebugRenderConstraints() {}
    void DebugRenderConstraintStiffness() {}
    void DebugRenderConstraintError() {}
    void DebugRenderPositionDelta() {}
    void DebugRenderBoundingBox() {}

    enum {
        DEBUG_VIS_DISTANCE_CONSTRAINTS = 1,
        DEBUG_VIS_TETHERS = 2,
        DEBUG_VIS_CONSTRAINTS = 4,
        DEBUG_VIS_CONSTRAINTS_STIFFNESS = 8,
        DEBUG_VIS_NORMALS = 16,
        DEBUG_VIS_TANGENTS = 32,
        DEBUG_VIS_BITANGENTS = 64,
        DEBUG_VIS_CONSTRAINT_ERROR = 128,
        DEBUG_VIS_POSITION_DELTA = 256,
        DEBUG_VIS_ACCELERATION = 512,
        DEBUG_VIS_BOUNDING_BOX = 1024,
        DEBUG_VIS_LAST
    };
    static unsigned int debug_visualization_flags_;
    static bool debug_visualization_update_requested_;

    struct SceneDebugRenderParams {
        // Constraint render params
        int visible_phase_range_begin;
        int visible_phase_range_end;
    };
    static SceneDebugRenderParams scene_debug_render_params_;
};

unsigned int ClothUI::debug_visualization_flags_ = 0;
bool ClothUI::debug_visualization_update_requested_ = true;
ClothUI::SceneDebugRenderParams ClothUI::scene_debug_render_params_;

}  // namespace

ClothInspector::ClothInspector(const std::string &title,
                               bool opened,
                               const PanelWindowSettings &window_settings,
                               cloth::ClothController &controller)
    : PanelWindow(title, opened, window_settings) {
    CreateWidget<ClothUI>(controller);
}

}  // namespace vox::editor::ui
