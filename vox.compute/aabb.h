//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.compute/common.h"

namespace vox::compute {
class AABB {
public:
    Vector3r m_p_[2];

    AABB& operator=(const AABB& aabb) {
        m_p_[0] = aabb.m_p_[0];
        m_p_[1] = aabb.m_p_[1];
        return *this;
    }

    static bool PointInAabb(const AABB& a, const Vector3r& p) {
        if ((p[0] < a.m_p_[0][0]) || (p[1] < a.m_p_[0][1]) || (p[2] < a.m_p_[0][2])) return false;
        if ((p[0] > a.m_p_[1][0]) || (p[1] > a.m_p_[1][1]) || (p[2] > a.m_p_[1][2])) return false;
        return true;
    }

    static void GetEdge(const AABB& a, char i, Vector3r& p_1, Vector3r& p_2) {
        char c_1, c_2;
        GetEdgeIndex(i, c_1, c_2);
        CornerPoint(a, c_1, p_1);
        CornerPoint(a, c_2, p_2);
    }

    static void GetEdgeIndex(char i, char& p_1, char& p_2) {
        //                         0    1    2    3    4    5    6    7    8    9    10   11
        static char index[12 * 2] = {0, 1, 0, 2, 1, 3, 2, 3, 0, 4, 1, 5, 2, 6, 3, 7, 4, 5, 4, 6, 5, 7, 6, 7};
        p_1 = index[2 * i + 0];
        p_2 = index[2 * i + 1];
    }

    static void CornerPoint(const AABB& a, char i, Vector3r& p) {
        switch (i) {
            case 0:
                p = Vector3r(a.m_p_[0][0], a.m_p_[0][1], a.m_p_[0][2]);
                break;
            case 1:
                p = Vector3r(a.m_p_[1][0], a.m_p_[0][1], a.m_p_[0][2]);
                break;
            case 2:
                p = Vector3r(a.m_p_[0][0], a.m_p_[1][1], a.m_p_[0][2]);
                break;
            case 3:
                p = Vector3r(a.m_p_[1][0], a.m_p_[1][1], a.m_p_[0][2]);
                break;
            case 4:
                p = Vector3r(a.m_p_[0][0], a.m_p_[0][1], a.m_p_[1][2]);
                break;
            case 5:
                p = Vector3r(a.m_p_[1][0], a.m_p_[0][1], a.m_p_[1][2]);
                break;
            case 6:
                p = Vector3r(a.m_p_[0][0], a.m_p_[1][1], a.m_p_[1][2]);
                break;
            case 7:
                p = Vector3r(a.m_p_[1][0], a.m_p_[1][1], a.m_p_[1][2]);
                break;
            default:
                break;
        }
    }

    static FORCE_INLINE bool Intersection(const AABB& a_1, const AABB& a_2) {
        for (char i = 0; i < 3; i++) {
            const Real kMin0 = a_1.m_p_[0][i];
            const Real kMax0 = a_1.m_p_[1][i];
            const Real kMin1 = a_2.m_p_[0][i];
            const Real kMax1 = a_2.m_p_[1][i];
            if (((kMax0 < kMin1) || (kMin0 > kMax1))) return false;
        }
        return true;
    }
};
}  // namespace vox::compute
