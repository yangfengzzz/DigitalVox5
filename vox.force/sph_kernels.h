//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <cmath>

#include "vox.force/common.h"

#define NO_DISTANCE_TEST

namespace vox::force {
class CubicKernel {
protected:
    static Real m_radius_;
    static Real m_k_;
    static Real m_l_;
    static Real m_w_zero_;

public:
    static Real GetRadius() { return m_radius_; }
    static void SetRadius(Real val) {
        m_radius_ = val;
        static const Real kPi = static_cast<Real>(M_PI);

        const Real kH3 = m_radius_ * m_radius_ * m_radius_;
        m_k_ = static_cast<Real>(8.0) / (kPi * kH3);
        m_l_ = static_cast<Real>(48.0) / (kPi * kH3);
        m_w_zero_ = W(Vector3r(0.0, 0.0, 0.0));
    }

public:
    // static unsigned int counter;
    static Real W(const Vector3r &r) {
        // counter++;
        Real res = 0.0;
        const Real kRl = r.norm();
        const Real kQ = kRl / m_radius_;
#ifndef NO_DISTANCE_TEST
        if (q <= 1.0)
#endif
        {
            if (kQ <= 0.5) {
                const Real kQ2 = kQ * kQ;
                const Real kQ3 = kQ2 * kQ;
                res = m_k_ * (static_cast<Real>(6.0) * kQ3 - static_cast<Real>(6.0) * kQ2 + static_cast<Real>(1.0));
            } else {
                res = m_k_ * (static_cast<Real>(2.0) * pow(static_cast<Real>(1.0) - kQ, 3.f));
            }
        }
        return res;
    }

    static Vector3r GradW(const Vector3r &r) {
        Vector3r res;
        const Real kRl = r.norm();
        const Real kQ = kRl / m_radius_;
#ifndef NO_DISTANCE_TEST
        if (q <= 1.0)
#endif
        {
            if (kRl > 1.0e-6) {
                const Vector3r kGradq = r * ((Real)1.0 / (kRl * m_radius_));
                if (kQ <= 0.5) {
                    res = m_l_ * kQ * ((Real)3.0 * kQ - (Real)2.0) * kGradq;
                } else {
                    const Real kFactor = static_cast<Real>(1.0) - kQ;
                    res = m_l_ * (-kFactor * kFactor) * kGradq;
                }
            }
        }
#ifndef NO_DISTANCE_TEST
        else
            res.zero();
#endif

        return res;
    }

    static Real WZero() { return m_w_zero_; }
};

}  // namespace vox::force