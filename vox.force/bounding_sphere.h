//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <utility>
#include <vector>

#include "vox.force/common.h"

namespace vox::force {
/**
 * \brief Computes smallest enclosing spheres of point sets using Welzl's algorithm
 */
class BoundingSphere {
public:
    /**
     * \brief default constructor sets the center and radius to zero.
     */
    BoundingSphere() : m_x_(Vector3r::Zero()), m_r_(0.0) {}

    /**
     * \brief constructor which sets the center and radius
     *
     * \param x	3d coordinates of the center point
     * \param r radius of the sphere
     */
    BoundingSphere(Vector3r x, const Real r) : m_x_(std::move(x)), m_r_(r) {}

    /**
     * \brief	constructs a sphere for one point (with radius 0)
     *
     * \param a	3d coordinates of point a
     */
    explicit BoundingSphere(const Vector3r& a) {
        m_x_ = a;
        m_r_ = 0.0;
    }

    /**
     * \brief	constructs the smallest enclosing sphere for two points
     *
     * \param a 3d coordinates of point a
     * \param b 3d coordinates of point b
     */
    BoundingSphere(const Vector3r& a, const Vector3r& b) {
        const Vector3r kBa = b - a;

        m_x_ = (a + b) * static_cast<Real>(0.5);
        m_r_ = static_cast<Real>(0.5) * kBa.norm();
    }

    /**
     * \brief	constructs the smallest enclosing sphere for three points
     *
     * \param a 3d coordinates of point a
     * \param b 3d coordinates of point b
     * \param c 3d coordinates of point c
     */
    BoundingSphere(const Vector3r& a, const Vector3r& b, const Vector3r& c) {
        const Vector3r kBa = b - a;
        const Vector3r kCa = c - a;
        const Vector3r kBaxca = kBa.cross(kCa);
        Vector3r r;
        Matrix3r t;
        t << kBa[0], kBa[1], kBa[2], kCa[0], kCa[1], kCa[2], kBaxca[0], kBaxca[1], kBaxca[2];

        r[0] = static_cast<Real>(0.5) * kBa.squaredNorm();
        r[1] = static_cast<Real>(0.5) * kCa.squaredNorm();
        r[2] = static_cast<Real>(0.0);

        m_x_ = t.inverse() * r;
        m_r_ = m_x_.norm();
        m_x_ += a;
    }

    /**
     * \brief constructs the smallest enclosing sphere for four points
     *
     * \param a 3d coordinates of point a
     * \param b 3d coordinates of point b
     * \param c 3d coordinates of point c
     * \param d 3d coordinates of point d
     */
    BoundingSphere(const Vector3r& a, const Vector3r& b, const Vector3r& c, const Vector3r& d) {
        const Vector3r kBa = b - a;
        const Vector3r kCa = c - a;
        const Vector3r kDa = d - a;
        Vector3r r;
        Matrix3r t;
        t << kBa[0], kBa[1], kBa[2], kCa[0], kCa[1], kCa[2], kDa[0], kDa[1], kDa[2];

        r[0] = static_cast<Real>(0.5) * kBa.squaredNorm();
        r[1] = static_cast<Real>(0.5) * kCa.squaredNorm();
        r[2] = static_cast<Real>(0.5) * kDa.squaredNorm();
        m_x_ = t.inverse() * r;
        m_r_ = m_x_.norm();
        m_x_ += a;
    }

    /**
     * \brief	constructs the smallest enclosing sphere a given point-set
     *
     * \param p vertices of the points
     */
    explicit BoundingSphere(const std::vector<Vector3r>& p) {
        m_r_ = 0;
        m_x_.setZero();
        SetPoints(p);
    }

    /**
     * \brief	Getter for the center of the sphere
     *
     * \return	const reference of the sphere center
     */
    [[nodiscard]] Vector3r const& X() const { return m_x_; }

    /**
     * \brief	Access function for center of the sphere
     *
     * \return	reference of the sphere center
     */
    Vector3r& X() { return m_x_; }

    /**
     * \brief	Getter for the radius
     *
     * \return	Radius of the sphere
     */
    [[nodiscard]] Real R() const { return m_r_; }

    /**
     * \brief	Access function for the radius
     *
     * \return	Reference to the radius of the sphere
     */
    Real& R() { return m_r_; }

    /**
     * \brief	constructs the smallest enclosing sphere a given point-set
     *
     * \param p vertices of the points
     */
    void SetPoints(const std::vector<Vector3r>& p) {
        // remove duplicates
        std::vector<Vector3r> v(p);
        std::sort(v.begin(), v.end(), [](const Vector3r& a, const Vector3r& b) {
            if (a[0] < b[0]) return true;
            if (a[0] > b[0]) return false;
            if (a[1] < b[1]) return true;
            if (a[1] > b[1]) return false;
            return (a[2] < b[2]);
        });
        v.erase(std::unique(v.begin(), v.end(), [](Vector3r& a, Vector3r& b) { return a.isApprox(b); }), v.end());

        Vector3r d;
        const int kN = int(v.size());

        // generate random permutation of the points and permute the points by epsilon to avoid corner cases
        const double kEpsilon = 1.0e-6;
        for (int i = kN - 1; i > 0; i--) {
            const Vector3r kEpsilonVec = kEpsilon * Vector3r::Random();
            const int kJ = static_cast<int>(floor(i * double(rand()) / RAND_MAX));
            d = v[i] + kEpsilonVec;
            v[i] = v[kJ] - kEpsilonVec;
            v[kJ] = d;
        }

        BoundingSphere s = BoundingSphere(v[0], v[1]);

        for (int i = 2; i < kN; i++) {
            // SES0
            d = v[i] - s.X();
            if (d.squaredNorm() > s.R() * s.R()) s = Ses1(i, v, v[i]);
        }

        m_x_ = s.m_x_;
        m_r_ = s.m_r_ +
               static_cast<Real>(
                       kEpsilon);  // add epsilon to make sure that all non-pertubated points are inside the sphere
    }

    /**
     * \brief		intersection test for two spheres
     *
     * \param other other sphere to be tested for intersection
     * \return		returns true when this sphere and the other sphere are intersecting
     */
    [[nodiscard]] bool Overlaps(BoundingSphere const& other) const {
        double rr = m_r_ + other.m_r_;
        return (m_x_ - other.m_x_).squaredNorm() < rr * rr;
    }

    /**
     * \brief		tests whether the given sphere other is contained in the sphere
     *
     * \param		other bounding sphere
     * \return		returns true when the other is contained in this sphere or vice versa
     */
    [[nodiscard]] bool Contains(BoundingSphere const& other) const {
        double rr = R() - other.R();
        return (X() - other.X()).squaredNorm() < rr * rr;
    }

    /**
     * \brief		tests whether the given point other is contained in the sphere
     *
     * \param		other 3d coordinates of a point
     * \return		returns true when the point is contained in the sphere
     */
    [[nodiscard]] bool Contains(Vector3r const& other) const { return (X() - other).squaredNorm() < m_r_ * m_r_; }

private:
    /**
     * \brief		constructs the smallest enclosing sphere for n points with the points q1, q2, and q3 on the
     * surface of the sphere
     *
     * \param n		number of points
     * \param p		vertices of the points
     * \param q_1	3d coordinates of a point on the surface
     * \param q_2	3d coordinates of a second point on the surface
     * \param q_3	3d coordinates of a third point on the surface
     * \return		smallest enclosing sphere
     */
    static BoundingSphere Ses3(int n, std::vector<Vector3r>& p, Vector3r& q_1, Vector3r& q_2, Vector3r& q_3) {
        BoundingSphere s(q_1, q_2, q_3);

        for (int i = 0; i < n; i++) {
            Vector3r d = p[i] - s.X();
            if (d.squaredNorm() > s.R() * s.R()) s = BoundingSphere(q_1, q_2, q_3, p[i]);
        }
        return s;
    }

    /**
     * \brief		constructs the smallest enclosing sphere for n points with the points q1 and q2 on the surface
     * of the sphere
     *
     * \param n		number of points
     * \param p		vertices of the points
     * \param q_1	3d coordinates of a point on the surface
     * \param q_2	3d coordinates of a second point on the surface
     * \return		smallest enclosing sphere
     */
    static BoundingSphere Ses2(int n, std::vector<Vector3r>& p, Vector3r& q_1, Vector3r& q_2) {
        BoundingSphere s(q_1, q_2);

        for (int i = 0; i < n; i++) {
            Vector3r d = p[i] - s.X();
            if (d.squaredNorm() > s.R() * s.R()) s = Ses3(i, p, q_1, q_2, p[i]);
        }
        return s;
    }
    /**
     * \brief		constructs the smallest enclosing sphere for n points with the point q1 on the surface of the
     * sphere
     *
     * \param n		number of points
     * \param p		vertices of the points
     * \param q_1	3d coordinates of a point on the surface
     * \return		smallest enclosing sphere
     */
    static BoundingSphere Ses1(int n, std::vector<Vector3r>& p, Vector3r& q_1) {
        BoundingSphere s(p[0], q_1);

        for (int i = 1; i < n; i++) {
            Vector3r d = p[i] - s.X();
            if (d.squaredNorm() > s.R() * s.R()) s = Ses2(i, p, q_1, p[i]);
        }
        return s;
    }

private:
    Vector3r m_x_;
    Real m_r_;
};

}  // namespace vox::force
