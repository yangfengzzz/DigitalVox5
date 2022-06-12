//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "vox.compute/common.h"

namespace vox::compute {
class VolumeIntegration {
public:
    VolumeIntegration(unsigned int n_vertices, unsigned int n_faces, Vector3r* vertices, const unsigned int* indices);

    /** Compute inertia tensor for given geometry and given density.
     */
    void ComputeInertiaTensor(Real density);

    /** Return mass of body. */
    [[nodiscard]] Real GetMass() const { return m_mass_; }
    /** Return volume of body. */
    [[nodiscard]] Real GetVolume() const { return m_volume_; }
    /** Return inertia tensor of body. */
    [[nodiscard]] Matrix3r const& GetInertia() const { return m_theta_; }
    /** Return center of mass. */
    [[nodiscard]] Vector3r const& GetCenterOfMass() const { return m_r_; }

private:
    int A{};
    int B{};
    int C{};

    // projection integrals
    Real P1{}, Pa{}, Pb{}, Paa{}, Pab{}, Pbb{}, Paaa{}, Paab{}, Pabb{}, Pbbb{};
    // face integrals
    Real Fa{}, Fb{}, Fc{}, Faa{}, Fbb{}, Fcc{}, Faaa{}, Fbbb{}, Fccc{}, Faab{}, Fbbc{}, Fcca{};
    // volume integrals
    Real T0{};
    Real T1[3]{};
    Real T2[3]{};
    Real TP[3]{};

private:
    void VolumeIntegrals();
    void FaceIntegrals(unsigned int i);

    /** Compute various integrations over projection of face.
     */
    void ProjectionIntegrals(unsigned int i);

    std::vector<Vector3r> m_face_normals_;
    std::vector<Real> m_weights_;
    unsigned int m_n_vertices_;
    unsigned int m_n_faces_;
    std::vector<Vector3r> m_vertices_;
    const unsigned int* m_indices_;

    Real m_mass_{}, m_volume_{};
    Vector3r m_r_;
    Vector3r m_x_;
    Matrix3r m_theta_;
};

}  // namespace vox::compute
