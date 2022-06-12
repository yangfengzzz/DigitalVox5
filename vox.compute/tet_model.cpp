//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.compute/tet_model.h"

#include "vox.base/logging.h"
#include "vox.compute/position_based_dynamics.h"

namespace vox::compute {

TetModel::TetModel() : m_surface_mesh_(), m_vis_vertices_(), m_vis_mesh_(), m_particle_mesh_() {
    m_initial_x_.setZero();
    m_initial_r_.setIdentity();
    m_initial_scale_.setOnes();
    m_restitution_coeff_ = static_cast<Real>(0.6);
    m_friction_coeff_ = static_cast<Real>(0.2);
}

TetModel::~TetModel() { CleanupModel(); }

void TetModel::CleanupModel() { m_particle_mesh_.Release(); }

TetModel::SurfaceMesh &TetModel::GetSurfaceMesh() { return m_surface_mesh_; }

VertexData &TetModel::GetVisVertices() { return m_vis_vertices_; }

TetModel::SurfaceMesh &TetModel::GetVisMesh() { return m_vis_mesh_; }

void TetModel::InitMesh(const unsigned int n_points,
                        const unsigned int n_tets,
                        const unsigned int index_offset,
                        unsigned int *indices) {
    m_index_offset_ = index_offset;
    m_particle_mesh_.Release();
    m_particle_mesh_.InitMesh(n_points, n_tets * 6, n_tets * 4, n_tets);

    for (unsigned int i = 0; i < n_tets; i++) {
        m_particle_mesh_.AddTet(&indices[4 * i]);
    }
    m_particle_mesh_.BuildNeighbors();

    CreateSurfaceMesh();
}

unsigned int TetModel::GetIndexOffset() const { return m_index_offset_; }

void TetModel::CreateSurfaceMesh() {
    const unsigned int nVerts = m_particle_mesh_.NumVertices();

    m_surface_mesh_.InitMesh(nVerts, m_particle_mesh_.NumEdges(), m_particle_mesh_.NumFaces());

    // Search for all border faces of the tet mesh
    const IndexedTetMesh::Face *faceData = m_particle_mesh_.GetFaceData().data();
    const unsigned int *faces = m_particle_mesh_.GetFaces().data();
    for (unsigned int i = 0; i < m_particle_mesh_.NumFaces(); i++) {
        const IndexedTetMesh::Face &face = faceData[i];
        // Found border face
        if ((face.m_tets[1] == 0xffffffff) || (face.m_tets[0] == 0xffffffff)) {
            m_surface_mesh_.AddFace(&faces[3 * i]);
        }
    }
    m_surface_mesh_.BuildNeighbors();
}

void TetModel::UpdateMeshNormals(const ParticleData &pd) {
    m_surface_mesh_.UpdateNormals(pd, m_index_offset_);
    m_surface_mesh_.UpdateVertexNormals(pd);
}

void TetModel::AttachVisMesh(const ParticleData &pd) {
    const Real eps = static_cast<Real>(1.0e-6);

    // The created surface mesh defines the boundary of the tet mesh
    unsigned int *faces = m_surface_mesh_.GetFaces().data();
    const unsigned int nFaces = m_surface_mesh_.NumFaces();

    const Vector3r *normals = m_surface_mesh_.GetVertexNormals().data();

    // for each point find nearest triangle (TODO: optimize)
    const int nNearstT = 15;
    m_attachments_.resize(m_vis_vertices_.Size());

#pragma omp parallel shared(nFaces, faces, pd, normals, eps) default(none)
    {
#pragma omp for schedule(static)
        for (int i = 0; i < (int)m_vis_vertices_.Size(); i++) {
            const Vector3r &p = m_vis_vertices_.GetPosition(i);
            Real curDist[nNearstT];
            int curT[nNearstT];
            for (int k = 0; k < nNearstT; k++) {
                curDist[k] = std::numeric_limits<Real>::max();
                curT[k] = -1;
            }
            Vector3r curBary[nNearstT];
            Vector3r curInter[nNearstT];
            for (unsigned int j = 0; j < nFaces; j++) {
                const unsigned int indexA = faces[3 * j] + m_index_offset_;
                const unsigned int indexB = faces[3 * j + 1] + m_index_offset_;
                const unsigned int indexC = faces[3 * j + 2] + m_index_offset_;
                const Vector3r &a = pd.GetPosition0(indexA);
                const Vector3r &b = pd.GetPosition0(indexB);
                const Vector3r &c = pd.GetPosition0(indexC);

                Vector3r inter, bary;
                // compute nearest point on triangle
                if (PointInTriangle(a, b, c, p, inter, bary)) {
                    Real len = (p - inter).norm();
                    for (int k = nNearstT - 1; k >= 0; k--)  // update the best triangles
                    {
                        if (len < curDist[k]) {
                            if (k < nNearstT - 1) {
                                curDist[k + 1] = curDist[k];
                                curBary[k + 1] = curBary[k];
                                curT[k + 1] = curT[k];
                                curInter[k + 1] = curInter[k];
                            }
                            curDist[k] = len;
                            curBary[k] = bary;
                            curT[k] = (int)j;
                            curInter[k] = inter;
                        }
                    }
                }
            }
            if (curT[0] == -1) {
                LOGE("ERROR: vertex has no nearest triangle.")
                continue;
            }

            // take the best bary coords we find from the best 5 triangles
            Real error = std::numeric_limits<Real>::max();
            int current_k = 0;
            Real current_dist = 0.0;
            Vector3r current_bary;
            for (int k = 0; k < nNearstT; k++) {
                if (curT[k] == -1) break;

                // see Kobbelt: Multiresolution Herarchies on unstructured triangle meshes
                const Vector3r &p = m_vis_vertices_.GetPosition(i);
                const Vector3r n1 = -normals[faces[3 * curT[k] + 0]];
                const Vector3r n2 = -normals[faces[3 * curT[k] + 1]];
                const Vector3r n3 = -normals[faces[3 * curT[k] + 2]];
                const Vector3r &p1 = pd.GetPosition0(faces[3 * curT[k] + 0] + m_index_offset_);
                const Vector3r &p2 = pd.GetPosition0(faces[3 * curT[k] + 1] + m_index_offset_);
                const Vector3r &p3 = pd.GetPosition0(faces[3 * curT[k] + 2] + m_index_offset_);
                const Vector3r U = p.cross(n1);
                const Vector3r V = p.cross(n2);
                const Vector3r W = p.cross(n3);
                const Vector3r UU = n1.cross(p1);
                const Vector3r VV = n2.cross(p2);
                const Vector3r WW = n3.cross(p3);
                const Vector3r UV = (n2.cross(p1)) + (n1.cross(p2));
                const Vector3r UW = (n3.cross(p1)) + (n1.cross(p3));
                const Vector3r VW = (n3.cross(p2)) + (n2.cross(p3));
                // F(u,v) = F + Fu*u + Fv*v + Fuu*u*u + Fuv*u*v + Fvv*v*v == 0!
                const Vector3r F = W + WW;
                const Vector3r Fu = U + UW - W - WW * 2.0;
                const Vector3r Fv = V + VW - W - WW * 2.0;
                const Vector3r Fuu = UU - UW + WW;
                const Vector3r Fuv = UV - UW - VW + WW * 2.0;
                const Vector3r Fvv = VV - VW + WW;
                Real u = curBary[k][0];
                Real v = curBary[k][0];
                SolveQuadraticForZero(F, Fu, Fv, Fuu, Fuv, Fvv, u, v);
                Real w = static_cast<Real>(1.0) - u - v;

                if (u < 0) u = 0.0;
                if (u > 1) u = 1.0;
                if (v < 0) v = 0.0;
                if (v > 1) v = 1.0;
                if (u + v > 1) {
                    Real uv = u + v;
                    Real u_ = u;
                    Real v_ = v;
                    u -= (uv - static_cast<Real>(1.0)) * v_ / uv;
                    v -= (uv - static_cast<Real>(1.0)) * u_ / uv;
                }
                w = static_cast<Real>(1.0) - u - v;
                Vector3r curInter = p1 * u + p2 * v + p3 * w;
                Real dist = (p - curInter).norm();

                Vector3r n = n1 * u + n2 * v + n3 * w;
                Real err = dist;
                if ((p - curInter).dot(n) < 0.0) dist *= -1.0;
                Vector3r interP = curInter + n * dist;
                err += (interP - p).norm();

                if (err > error) continue;

                error = err;

                current_k = k;
                current_dist = dist;
                current_bary = Vector3r(u, v, w);

                if (error < eps) break;
            }

            Attachment &fp = m_attachments_[i];
            fp.m_index = i;
            fp.m_tri_index = (unsigned int)curT[current_k];
            fp.m_bary[0] = current_bary.x();
            fp.m_bary[1] = current_bary.y();
            fp.m_bary[2] = current_bary.z();
            fp.m_dist = current_dist;
            fp.m_min_error = error;
        }
    }
}

void TetModel::SolveQuadraticForZero(const Vector3r &F,
                                     const Vector3r &Fu,
                                     const Vector3r &Fv,
                                     const Vector3r &Fuu,
                                     const Vector3r &Fuv,
                                     const Vector3r &Fvv,
                                     Real &u,
                                     Real &v) {
    // newton iterations search F(u,v) = [0,0,0]
    Real eps = static_cast<Real>(1.0e-6);
    unsigned char k;
    for (k = 0; k < 50; k++) {
        // x(n+1) = x(n) - F'^(-1)(x(n))*F(x(n))
        // dx = -F'^(-1)*F => dF*dx = -F
        // => dF^T*dF*dx = dF^T*(-F)
        // solve for dx
        const Vector3r f = -(F + Fu * u + Fv * v + Fuu * u * u + Fuv * u * v + Fvv * v * v);
        if ((fabs(f[0]) < eps) && (fabs(f[1]) < eps) && (fabs(f[2]) < eps)) break;
        Vector3r dF[2];
        dF[0] = Fu + Fuu * (u * 2) + Fuv * v;
        dF[1] = Fv + Fvv * (v * 2) + Fuv * u;
        Real dFdF[3];
        dFdF[0] = dF[0].dot(dF[0]);
        dFdF[1] = dF[0].dot(dF[1]);
        dFdF[2] = dF[1].dot(dF[1]);
        const Real det = dFdF[0] * dFdF[2] - dFdF[1] * dFdF[1];
        if (fabs(det) < eps) break;
        Real H[3];
        H[0] = dFdF[2] / det;
        H[1] = -dFdF[1] / det;
        H[2] = dFdF[0] / det;
        const Real h1 = dF[0].dot(f);
        const Real h2 = dF[1].dot(f);
        u += H[0] * h1 + H[1] * h2;
        v += H[1] * h1 + H[2] * h2;
    }
}

void TetModel::UpdateVisMesh(const ParticleData &pd) {
    if (m_attachments_.empty()) return;

    // The collision mesh is the boundary of the tet mesh
    unsigned int *faces = m_surface_mesh_.GetFaces().data();
    const unsigned int nFaces = m_surface_mesh_.NumFaces();

    const Vector3r *normals = m_surface_mesh_.GetVertexNormals().data();

#pragma omp parallel shared(faces, pd, normals) default(none)
    {
#pragma omp for schedule(static)
        for (auto &m_attachment : m_attachments_) {
            const unsigned int pindex = m_attachment.m_index;
            const unsigned int triindex = m_attachment.m_tri_index;
            const Real *bary = m_attachment.m_bary;

            const unsigned int indexA = faces[3 * triindex] + m_index_offset_;
            const unsigned int indexB = faces[3 * triindex + 1] + m_index_offset_;
            const unsigned int indexC = faces[3 * triindex + 2] + m_index_offset_;

            const Vector3r &a = pd.GetPosition(indexA);
            const Vector3r &b = pd.GetPosition(indexB);
            const Vector3r &c = pd.GetPosition(indexC);
            Vector3r p2 = bary[0] * a + bary[1] * b + bary[2] * c;
            Vector3r n = bary[0] * normals[faces[3 * triindex]] + bary[1] * normals[faces[3 * triindex + 1]] +
                         bary[2] * normals[faces[3 * triindex + 2]];
            n.normalize();

            Vector3r &p = m_vis_vertices_.GetPosition(pindex);
            p = p2 - n * m_attachment.m_dist;
        }
    }

    m_vis_mesh_.UpdateNormals(m_vis_vertices_, 0);
    m_vis_mesh_.UpdateVertexNormals(m_vis_vertices_);
}

bool TetModel::PointInTriangle(const Vector3r &p0,
                               const Vector3r &p1,
                               const Vector3r &p2,
                               const Vector3r &p,
                               Vector3r &inter,
                               Vector3r &bary) {
    // see Bridson: Robust treatment of collisions contact and friction for cloth animation
    const Vector3r x43 = p - p2;
    const Vector3r x13 = p0 - p2;
    const Vector3r x23 = p1 - p2;

    // compute inv matrix a,b,b,c
    Real a = x13.dot(x13);
    Real b = x13.dot(x23);
    Real c = x23.dot(x23);
    const Real det = a * c - b * b;
    if (fabs(det) < 1.0e-9) return false;

    Real d1 = x13.dot(x43);
    Real d2 = x23.dot(x43);

    Real w1 = (c * d1 - b * d2) / det;
    Real w2 = (a * d2 - b * d1) / det;

    // this clamping gives not an exact orthogonal point to the edge!!
    if (w1 < 0) w1 = 0;
    if (w1 > 1) w1 = 1;
    if (w2 < 0) w2 = 0;
    if (w2 > 1) w2 = 1;

    bary[0] = w1;
    bary[1] = w2;
    bary[2] = (Real)1 - w1 - w2;

    if (bary[2] < 0) {
        // this gives not an exact orthogonal point to the edge!!
        const Real w12 = w1 + w2;
        bary[0] -= w2 / (w12) * (w12 - 1);
        bary[1] -= w1 / (w12) * (w12 - 1);
        bary[2] = 0;
    }

    inter = p2 + bary[0] * x13 + bary[1] * x23;

    return true;
}

}  // namespace vox::compute
