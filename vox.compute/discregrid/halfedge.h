//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cassert>

namespace vox::compute::discregrid {

class Halfedge {
public:
    Halfedge() : m_code_(3) {}
    Halfedge(Halfedge const&) = default;
    Halfedge(unsigned int f, unsigned char e) : m_code_((f << 2) | e) {
        // assert(e < 3);
    }

    [[nodiscard]] Halfedge Next() const { return Halfedge(Face(), (Edge() + 1) % 3); }

    [[nodiscard]] Halfedge Previous() const { return Halfedge(Face(), (Edge() + 2) % 3); }

    bool operator==(Halfedge const& other) const { return m_code_ == other.m_code_; }

    [[nodiscard]] unsigned int Face() const { return m_code_ >> 2; }
    [[nodiscard]] unsigned char Edge() const { return m_code_ & 0x3; }
    [[nodiscard]] bool IsBoundary() const { return Edge() == 3; }

private:
    explicit Halfedge(unsigned int code) : m_code_(code) {}
    unsigned int m_code_;
};
}  // namespace vox::compute::discregrid
