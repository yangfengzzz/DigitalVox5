//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cassert>

namespace vox::force::discregrid {

class Halfedge {
public:
    Halfedge() : m_code(3) {}
    Halfedge(Halfedge const&) = default;
    Halfedge(unsigned int f, unsigned char e) : m_code((f << 2) | e) {
        // assert(e < 3);
    }

    [[nodiscard]] Halfedge next() const { return Halfedge(face(), (edge() + 1) % 3); }

    [[nodiscard]] Halfedge previous() const { return Halfedge(face(), (edge() + 2) % 3); }

    bool operator==(Halfedge const& other) const { return m_code == other.m_code; }

    [[nodiscard]] unsigned int face() const { return m_code >> 2; }
    [[nodiscard]] unsigned char edge() const { return m_code & 0x3; }
    [[nodiscard]] bool isBoundary() const { return edge() == 3; }

private:
    explicit Halfedge(unsigned int code) : m_code(code) {}
    unsigned int m_code;
};
}  // namespace vox::force::discregrid
