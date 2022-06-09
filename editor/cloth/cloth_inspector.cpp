//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "editor/cloth/cloth_inspector.h"

#include "vox.cloth/NvCloth/Factory.h"
#include "vox.render/entity.h"
#include "vox.render/wireframe/wireframe_manager.h"

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

    void DebugRenderDistanceConstraints() {
        const auto kDebugManager = WireframeManager::GetSingletonPtr();

        for (auto it : controller_.ClothList()) {
            nv::cloth::Cloth &cloth = *it->cloth_;
            if (cloth.getNumMotionConstraints() == 0) continue;

            nv::cloth::Factory &factory = cloth.getFactory();

            Matrix4x4F transform = it->GetEntity()->transform->WorldMatrix();

            nv::cloth::MappedRange<physx::PxVec4> particles = cloth.getCurrentParticles();
            std::vector<physx::PxVec4> motion_constraints;
            motion_constraints.reserve(cloth.getNumMotionConstraints() * 2);
            motion_constraints.resize(cloth.getNumMotionConstraints());
            factory.extractMotionConstraints(
                    cloth, nv::cloth::Range<physx::PxVec4>(&motion_constraints[0],
                                                           &motion_constraints[0] + motion_constraints.size()));
            motion_constraints.resize(cloth.getNumMotionConstraints() * 2);

            nv::cloth::MappedRange<physx::PxVec4> positions = cloth.getCurrentParticles();

            assert(positions.size() == cloth.getNumMotionConstraints());

            // Set to 1 to color code lines based on distance constraint length (as % of max constraint distance in
            // cloth) Set to 0 to color code lines based on how close the particle is to the distance constraint (as %
            // of max distance per constraint)
#define SHOW_DISTANCE_COLOR 0
#if SHOW_DISTANCE_COLOR
            float maxDist = 0.0f;
            for (int i = (int)(motionConstraints.size() >> 1) - 1; i >= 0; i--) {
                maxDist = max(maxDist, motionConstraints[i].w);
            }
#endif

            for (int i = ((int)motion_constraints.size() >> 1) - 1; i >= 0; i--) {
                float l = motion_constraints[i].w;
                physx::PxVec3 a = motion_constraints[i].getXYZ();
                physx::PxVec3 b = positions[i].getXYZ();
                physx::PxVec3 d = b - a;
                float current_dist = d.magnitude();
                if (d.magnitudeSquared() < 0.00001f) {
                    d = physx::PxVec3(0.0f, 0.0f, 1.0f);
                } else {
                    d.normalize();
                }
                unsigned char clerp;
#if SHOW_DISTANCE_COLOR
                clerp = (unsigned char)(std::max(0.0f, std::min(1.0f, (l / maxDist))) * 255.0f);
#else
                clerp = (unsigned char)(std::max(0.0f, std::min(1.0f, (current_dist / l))) * 255.0f);
#endif
                unsigned int c = ((255 - clerp) << 8) + clerp;

                physx::PxVec3 temp = a + d * l;
                kDebugManager->AddLine(transform, Vector3F(a.x, a.y, a.z), Vector3F(temp.x, temp.y, temp.z), c);
            }
        }
    }

    void DebugRenderTethers() {
        const auto kDebugManager = WireframeManager::GetSingletonPtr();

        for (auto it : controller_.ClothList()) {
            nv::cloth::Cloth &cloth = *it->cloth_;
            nv::cloth::Fabric &fabric = cloth.getFabric();
            if (fabric.getNumTethers() == 0) continue;

            nv::cloth::Factory &factory = cloth.getFactory();

            Matrix4x4F transform = it->GetEntity()->transform->WorldMatrix();

            nv::cloth::MappedRange<physx::PxVec4> particles = cloth.getCurrentParticles();

            std::vector<float> tether_lengths;
            tether_lengths.resize(fabric.getNumTethers());
            std::vector<uint32_t> anchors;
            anchors.resize(fabric.getNumTethers());

            factory.extractFabricData(
                    fabric, nv::cloth::Range<uint32_t>(nullptr, nullptr), nv::cloth::Range<uint32_t>(nullptr, nullptr),
                    nv::cloth::Range<float>(nullptr, nullptr), nv::cloth::Range<float>(nullptr, nullptr),
                    nv::cloth::Range<uint32_t>(nullptr, nullptr),
                    nv::cloth::Range<uint32_t>(&anchors[0], &anchors[0] + anchors.size()),
                    nv::cloth::Range<float>(&tether_lengths[0], &tether_lengths[0] + tether_lengths.size()),
                    nv::cloth::Range<uint32_t>(nullptr, nullptr));

            uint32_t particle_count = fabric.getNumParticles();

            for (int i = 0; i < (int)anchors.size(); i++) {
                float length_diff =
                        (particles[anchors[i]].getXYZ() - particles[i].getXYZ()).magnitude() - tether_lengths[i];

                physx::PxVec3 p_0_temp = particles[anchors[i]].getXYZ();
                physx::PxVec3 p_1_temp = particles[i % particle_count].getXYZ();
                kDebugManager->AddLine(transform, Vector3F(p_0_temp.x, p_0_temp.y, p_0_temp.z),
                                       Vector3F(p_1_temp.x, p_1_temp.y, p_1_temp.z),
                                       length_diff > 0.0f ? 0x0000FF : 0x00FFFF);
            }
        }
    }

    void DebugRenderConstraints() {
        const auto kDebugManager = WireframeManager::GetSingletonPtr();

        for (auto it : controller_.ClothList()) {
            nv::cloth::Cloth &cloth = *it->cloth_;
            nv::cloth::Fabric &fabric = cloth.getFabric();
            if (fabric.getNumIndices() == 0) continue;

            nv::cloth::Factory &factory = cloth.getFactory();

            Matrix4x4F transform = it->GetEntity()->transform->WorldMatrix();

            nv::cloth::MappedRange<physx::PxVec4> particles = cloth.getCurrentParticles();

            if (scene_debug_render_params_.visible_phase_range_begin >=
                scene_debug_render_params_.visible_phase_range_end) {
                // then simply render all constraints in one go
                std::vector<uint32_t> indices;
                indices.resize(fabric.getNumIndices());

                factory.extractFabricData(
                        fabric, nv::cloth::Range<uint32_t>(nullptr, nullptr),
                        nv::cloth::Range<uint32_t>(nullptr, nullptr), nv::cloth::Range<float>(nullptr, nullptr),
                        nv::cloth::Range<float>(nullptr, nullptr),
                        nv::cloth::Range<uint32_t>(&indices[0], &indices[0] + indices.size()),
                        nv::cloth::Range<uint32_t>(nullptr, nullptr), nv::cloth::Range<float>(nullptr, nullptr),
                        nv::cloth::Range<uint32_t>(nullptr, nullptr));

                for (int i = 1; i < (int)indices.size(); i += 2) {
                    physx::PxVec3 p_0_temp = particles[indices[i]].getXYZ();
                    physx::PxVec3 p_1_temp = particles[indices[i - 1]].getXYZ();
                    kDebugManager->AddLine(transform, Vector3F(p_0_temp.x, p_0_temp.y, p_0_temp.z),
                                           Vector3F(p_1_temp.x, p_1_temp.y, p_1_temp.z), 0x991919);
                }
            } else {
                // otherwise we render individual phases
                std::vector<uint32_t> indices;
                indices.resize(fabric.getNumIndices());
                std::vector<uint32_t> phases;
                phases.resize(fabric.getNumPhases());
                std::vector<uint32_t> sets;
                sets.resize(fabric.getNumSets());

                factory.extractFabricData(
                        fabric, nv::cloth::Range<uint32_t>(&phases[0], &phases[0] + phases.size()),
                        nv::cloth::Range<uint32_t>(&sets[0], &sets[0] + sets.size()),
                        nv::cloth::Range<float>(nullptr, nullptr), nv::cloth::Range<float>(nullptr, nullptr),
                        nv::cloth::Range<uint32_t>(&indices[0], &indices[0] + indices.size()),
                        nv::cloth::Range<uint32_t>(nullptr, nullptr), nv::cloth::Range<float>(nullptr, nullptr),
                        nv::cloth::Range<uint32_t>(nullptr, nullptr));

                uint32_t *i_it = &indices[0];
                for (int phase_index = 0; phase_index < (int)fabric.getNumPhases(); phase_index++) {
                    uint32_t *s_it = &sets[phases[phase_index]];
                    uint32_t *i_end = &indices[0] + (s_it[0] * 2);
                    uint32_t *i_start = i_it;

                    if (!(phase_index >= scene_debug_render_params_.visible_phase_range_begin &&
                          phase_index < scene_debug_render_params_.visible_phase_range_end)) {
                        i_it = i_end;
                        continue;
                    }

                    for (; i_it < i_end; i_it += 2) {
                        float c = (float)(i_it - i_start) / (float)(i_end - i_start);
                        auto c_255 = (unsigned char)(c * 255.0f);

                        unsigned int color_table[3]{0xFF0000, 0x00FF00, 0x0000FF};
                        unsigned int shift_table[3]{8, 0, 16};

                        physx::PxVec3 p_0_temp = particles[*i_it].getXYZ();
                        physx::PxVec3 p_1_temp = particles[*(i_it + 1)].getXYZ();
                        kDebugManager->AddLine(transform, Vector3F(p_0_temp.x, p_0_temp.y, p_0_temp.z),
                                               Vector3F(p_1_temp.x, p_1_temp.y, p_1_temp.z),
                                               color_table[phase_index % 3] + (c_255 << shift_table[phase_index % 3]));
                    }
                }
            }
        }
    }

    void DebugRenderConstraintStiffness() {
        const auto kDebugManager = WireframeManager::GetSingletonPtr();

        for (auto it : controller_.ClothList()) {
            nv::cloth::Cloth &cloth = *it->cloth_;
            nv::cloth::Fabric &fabric = cloth.getFabric();
            if (fabric.getNumIndices() == 0) continue;

            if (!fabric.getNumStiffnessValues()) continue;

            nv::cloth::Factory &factory = cloth.getFactory();

            Matrix4x4F transform = it->GetEntity()->transform->WorldMatrix();

            nv::cloth::MappedRange<physx::PxVec4> particles = cloth.getCurrentParticles();

            std::vector<uint32_t> indices;
            indices.resize(fabric.getNumIndices());
            std::vector<float> stiffness;
            stiffness.resize(fabric.getNumRestvalues());

            factory.extractFabricData(
                    fabric, nv::cloth::Range<uint32_t>(nullptr, nullptr), nv::cloth::Range<uint32_t>(nullptr, nullptr),
                    nv::cloth::Range<float>(nullptr, nullptr),
                    nv::cloth::Range<float>(&stiffness[0], &stiffness[0] + stiffness.size()),
                    nv::cloth::Range<uint32_t>(&indices[0], &indices[0] + indices.size()),
                    nv::cloth::Range<uint32_t>(nullptr, nullptr), nv::cloth::Range<float>(nullptr, nullptr),
                    nv::cloth::Range<uint32_t>(nullptr, nullptr));

            for (int i = 1; i < (int)indices.size(); i += 2) {
                float c = 1.0f - exp2f(stiffness[i >> 1]);

                auto ca_255 = (unsigned char)(c * 255.0f * 0.8f);
                auto cb_255 = (unsigned char)((1.0f - c) * 255.0f * 0.8f);

                physx::PxVec3 p_0_temp = particles[indices[i - 1]].getXYZ();
                physx::PxVec3 p_1_temp = particles[indices[i]].getXYZ();
                kDebugManager->AddLine(transform, Vector3F(p_0_temp.x, p_0_temp.y, p_0_temp.z),
                                       Vector3F(p_1_temp.x, p_1_temp.y, p_1_temp.z), (ca_255 << 8) + (cb_255 << 0));
            }
        }
    }

    void DebugRenderConstraintError() {
        const auto kDebugManager = WireframeManager::GetSingletonPtr();

        for (auto it : controller_.ClothList()) {
            nv::cloth::Cloth &cloth = *it->cloth_;
            nv::cloth::Fabric &fabric = cloth.getFabric();
            if (fabric.getNumRestvalues() == 0) {
                continue;
            }
            nv::cloth::Factory &factory = cloth.getFactory();

            Matrix4x4F transform = it->GetEntity()->transform->WorldMatrix();

            nv::cloth::MappedRange<physx::PxVec4> particles = cloth.getCurrentParticles();

            std::vector<uint32_t> indices;
            indices.resize(fabric.getNumIndices());
            std::vector<float> rest_lengths;
            rest_lengths.resize(fabric.getNumRestvalues());

            factory.extractFabricData(
                    fabric, nv::cloth::Range<uint32_t>(nullptr, nullptr), nv::cloth::Range<uint32_t>(nullptr, nullptr),
                    nv::cloth::Range<float>(&rest_lengths[0], &rest_lengths[0] + rest_lengths.size()),
                    nv::cloth::Range<float>(nullptr, nullptr),
                    nv::cloth::Range<uint32_t>(&indices[0], &indices[0] + indices.size()),
                    nv::cloth::Range<uint32_t>(nullptr, nullptr), nv::cloth::Range<float>(nullptr, nullptr),
                    nv::cloth::Range<uint32_t>(nullptr, nullptr));

            for (int i = 0; i < (int)indices.size(); i += 2) {
                physx::PxVec4 p_0 = particles[indices[i]];
                physx::PxVec4 p_1 = particles[indices[i + 1]];
                float rest_length = rest_lengths[i >> 1];
                float length = (p_0 - p_1).magnitude();
                const float kScale = 2.0f;
                float error = (length / rest_length * 0.5f - 0.5f) * kScale + 0.5f;
                error = std::max(0.0f, std::min(1.0f, error));
                auto c_255 = (unsigned char)(error * 255.0f * 0.8f);

                physx::PxVec3 p_0_temp = p_0.getXYZ();
                physx::PxVec3 p_1_temp = p_1.getXYZ();
                kDebugManager->AddLine(transform, Vector3F(p_0_temp.x, p_0_temp.y, p_0_temp.z),
                                       Vector3F(p_1_temp.x, p_1_temp.y, p_1_temp.z),
                                       ((255 - c_255) << 8) + (c_255 << 0));
            }
        }
    }

    void DebugRenderPositionDelta() {
        const auto kDebugManager = WireframeManager::GetSingletonPtr();

        for (auto it : controller_.ClothList()) {
            nv::cloth::Cloth &cloth = *it->cloth_;

            Matrix4x4F transform = it->GetEntity()->transform->WorldMatrix();

            nv::cloth::MappedRange<physx::PxVec4> particles_1 = cloth.getCurrentParticles();
            nv::cloth::MappedRange<physx::PxVec4> particles_0 = cloth.getPreviousParticles();

            std::vector<physx::PxVec4> lines;

            // scale so that the solver frequency doesn't affect the position delta length assuming 60fps
            float iterations_per_frame = std::max(1.f, 1.0f / 60.0f * cloth.getSolverFrequency() + 0.5f);

            for (int i = 0; i < (int)particles_1.size(); i++) {
                physx::PxVec3 o = particles_1[i].getXYZ();
                physx::PxVec3 d = (particles_1[i] - particles_0[i]).getXYZ();
                kDebugManager->AddVector(transform, Vector3F(o.x, o.y, o.z),
                                         Vector3F(d.x, d.y, d.z) * iterations_per_frame * 2.0f,
                                         WireframeManager::FrameColor::RGB_RED);
            }
        }
    }

    void DebugRenderBoundingBox() {
        const auto kDebugManager = WireframeManager::GetSingletonPtr();

        for (auto it : controller_.ClothList()) {
            nv::cloth::Cloth &cloth = *it->cloth_;

            Matrix4x4F transform = it->GetEntity()->transform->WorldMatrix();

            physx::PxVec3 c_temp = cloth.getBoundingBoxCenter();
            Vector3F c = Vector3F(c_temp.x, c_temp.y, c_temp.z);
            physx::PxVec3 d_temp = cloth.getBoundingBoxScale();
            Vector3F d = Vector3F(d_temp.x, d_temp.y, d_temp.z);
            Vector3F dx = Vector3F(d.x, 0.0f, 0.0f);
            Vector3F dy = Vector3F(0.0f, d.y, 0.0f);
            Vector3F dz = Vector3F(0.0f, 0.0f, d.z);

            kDebugManager->AddLine(transform, c + dy + dz - dx, c + dy + dz + dx,
                                   WireframeManager::FrameColor::RGB_DARKGREEN);
            kDebugManager->AddLine(transform, c + dy - dz - dx, c + dy - dz + dx,
                                   WireframeManager::FrameColor::RGB_DARKGREEN);
            kDebugManager->AddLine(transform, c - dy + dz - dx, c - dy + dz + dx,
                                   WireframeManager::FrameColor::RGB_DARKGREEN);
            kDebugManager->AddLine(transform, c - dy - dz - dx, c - dy - dz + dx,
                                   WireframeManager::FrameColor::RGB_DARKGREEN);
            kDebugManager->AddLine(transform, c + dy + dx - dz, c + dy + dx + dz,
                                   WireframeManager::FrameColor::RGB_DARKGREEN);
            kDebugManager->AddLine(transform, c + dy - dx - dz, c + dy - dx + dz,
                                   WireframeManager::FrameColor::RGB_DARKGREEN);
            kDebugManager->AddLine(transform, c - dy + dx - dz, c - dy + dx + dz,
                                   WireframeManager::FrameColor::RGB_DARKGREEN);
            kDebugManager->AddLine(transform, c - dy - dx - dz, c - dy - dx + dz,
                                   WireframeManager::FrameColor::RGB_DARKGREEN);
            kDebugManager->AddLine(transform, c + dz + dx - dy, c + dz + dx + dy,
                                   WireframeManager::FrameColor::RGB_DARKGREEN);
            kDebugManager->AddLine(transform, c + dz - dx - dy, c + dz - dx + dy,
                                   WireframeManager::FrameColor::RGB_DARKGREEN);
            kDebugManager->AddLine(transform, c - dz + dx - dy, c - dz + dx + dy,
                                   WireframeManager::FrameColor::RGB_DARKGREEN);
            kDebugManager->AddLine(transform, c - dz - dx - dy, c - dz - dx + dy,
                                   WireframeManager::FrameColor::RGB_DARKGREEN);
            kDebugManager->AddLine(transform, c + dy + dz + dx, c - dy - dz - dx,
                                   WireframeManager::FrameColor::RGB_DARKGREEN);
            kDebugManager->AddLine(transform, c + dy + dz - dx, c - dy - dz + dx,
                                   WireframeManager::FrameColor::RGB_DARKGREEN);
            kDebugManager->AddLine(transform, c - dy + dz + dx, c + dy - dz - dx,
                                   WireframeManager::FrameColor::RGB_DARKGREEN);
            kDebugManager->AddLine(transform, c - dy + dz - dx, c + dy - dz + dx,
                                   WireframeManager::FrameColor::RGB_DARKGREEN);
        }
    }

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
