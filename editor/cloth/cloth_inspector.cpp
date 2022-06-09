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
    ClothUI(cloth::ClothController& controller):controller_(controller) {}
    
    void DrawImpl() override {
        _updateClothUI();
        _updateSolverUI();
        _updateDebugUI();
        
        _drawDebugVisualization();
    }
    
private:
    cloth::ClothController& controller_;
    
    void _updateClothUI() {
        static int activeCloth = 0;
        
        if (ImGui::TreeNode("Cloth Properties")) {
            activeCloth = std::min(activeCloth, (int) controller_.cloth_list().size() - 1);
            for (int i = 0; i < (int) controller_.cloth_list().size(); i++) {
                if (i)
                    ImGui::SameLine();
                std::stringstream clothName;
                clothName << "Cloth " << i;
                ImGui::RadioButton(clothName.str().c_str(), &activeCloth, i);
            }
            
            nv::cloth::Cloth *cloth = controller_.cloth_list()[activeCloth]->cloth_;
            {
                bool b = cloth->isContinuousCollisionEnabled();
                if (ImGui::Checkbox("Continuous Collision Detection (CCD)", &b))
                    cloth->enableContinuousCollision(b);
            }
            {
                physx::PxVec3 f3 = cloth->getDamping();
                if (ImGui::DragFloat3("Damping", &f3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setDamping(f3);
                float f = f3.maxElement();
                if (ImGui::DragFloat("Damping xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setDamping(physx::PxVec3(f, f, f));
            }
            {
                float f = cloth->getDragCoefficient();
                if (ImGui::DragFloat("Drag Coefficient", &f, 0.02f, 0.0f, 0.99f, "%.2f"))
                    cloth->setDragCoefficient(f);
            }
            {
                float f = cloth->getFriction();
                if (ImGui::DragFloat("Friction", &f, 0.04f, 0.0f, 2.0f, "%.2f"))
                    cloth->setFriction(f);
            }
            {
                physx::PxVec3 f3 = cloth->getGravity();
                if (ImGui::DragFloat3("Gravity", &f3.x, 0.5f, -50.0f, 50.0f, "%.1f"))
                    cloth->setGravity(f3);
            }
            {
                float f = cloth->getLiftCoefficient();
                if (ImGui::DragFloat("Lift Coefficient", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setLiftCoefficient(f);
            }
            {
                physx::PxVec3 f3 = cloth->getLinearInertia();
                if (ImGui::DragFloat3("Linear Inertia", &f3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setLinearInertia(f3);
                float f = f3.maxElement();
                if (ImGui::DragFloat("Linear Inertia xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setLinearInertia(physx::PxVec3(f, f, f));
            }
            {
                physx::PxVec3 f3 = cloth->getAngularInertia();
                if (ImGui::DragFloat3("Angular Inertia", &f3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setAngularInertia(f3);
                float f = f3.maxElement();
                if (ImGui::DragFloat("Angular Inertia xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setAngularInertia(physx::PxVec3(f, f, f));
            }
            {
                physx::PxVec3 f3 = cloth->getCentrifugalInertia();
                if (ImGui::DragFloat3("Centrifugal Inertia", &f3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setCentrifugalInertia(f3);
                float f = f3.maxElement();
                if (ImGui::DragFloat("Centrifugal Inertia xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setCentrifugalInertia(physx::PxVec3(f, f, f));
            }
            {
                physx::PxVec3 f3 = cloth->getLinearDrag();
                if (ImGui::DragFloat3("Linear Drag", &f3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setLinearDrag(f3);
                float f = f3.maxElement();
                if (ImGui::DragFloat("Linear Drag xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setLinearDrag(physx::PxVec3(f, f, f));
            }
            {
                physx::PxVec3 f3 = cloth->getAngularDrag();
                if (ImGui::DragFloat3("Angular Drag", &f3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setAngularDrag(f3);
                float f = f3.maxElement();
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
                if (ImGui::DragFloat("Sleep Threshold", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                    cloth->setSleepThreshold(f);
            }
            {
                float f = cloth->getStiffnessFrequency();
                if (ImGui::DragFloat("Stiffness Frequency", &f, 1.0f, 0.0f, 600.0f, "%.0f", ImGuiSliderFlags_Logarithmic))
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
                physx::PxVec3 f3 = cloth->getWindVelocity();
                if (ImGui::DragFloat3("Wind Velocity", &f3.x, 0.5f, -50.0f, 50.0f, "%.1f"))
                    cloth->setWindVelocity(f3);
            }
            ImGui::TreePop();
        }
    }
    
    void _updateSolverUI() {}

    void _updateDebugUI() {}

    void _drawDebugVisualization() {}
    
    void _debugRenderDistanceConstraints() {}
    void _debugRenderTethers() {}
    void _debugRenderConstraints() {}
    void _debugRenderConstraintStiffness() {}
    void _debugRenderConstraintError() {}
    void _debugRenderPositionDelta() {}
    void _debugRenderBoundingBox() {}
    
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
    static unsigned int _debugVisualizationFlags;
    static bool _debugVisualizationUpdateRequested;
    
    struct SceneDebugRenderParams {
        //Constraint render params
        int visiblePhaseRangeBegin;
        int visiblePhaseRangeEnd;
    };
    static SceneDebugRenderParams _sceneDebugRenderParams;
};

unsigned int ClothUI::_debugVisualizationFlags = 0;
bool ClothUI::_debugVisualizationUpdateRequested = true;
ClothUI::SceneDebugRenderParams ClothUI::_sceneDebugRenderParams;

}  // namespace

ClothInspector::ClothInspector(const std::string &title, bool opened, const PanelWindowSettings &window_settings,
                               cloth::ClothController& controller)
    : PanelWindow(title, opened, window_settings) {
    CreateWidget<ClothUI>(controller);
}

}  // namespace vox::editor::ui
