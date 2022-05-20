//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "color.h"

namespace vox {
const Color Color::Red = {1.f, 0.f, 0.f};
const Color Color::Green = {0.f, 1.f, 0.f};
const Color Color::Blue = {0.f, 0.f, 1.f};
const Color Color::White = {1.f, 1.f, 1.f};
const Color Color::Black = {0.f, 0.f, 0.f};
const Color Color::Grey = {0.5f, 0.5f, 0.5f};
const Color Color::Yellow = {1.f, 1.f, 0.f};
const Color Color::Cyan = {0.f, 1.f, 1.f};
const Color Color::Magenta = {1.f, 0.f, 1.f};

Color Color::lightened(float amount) const {
    amount = std::max(0.0f, std::min(1.0f, amount));
    return {(1.0f - amount) * r + amount * 1.0f, (1.0f - amount) * g + amount * 1.0f, (1.0f - amount) * +amount * 1.0f,
            a};
}

unsigned int Color::toABGR32() const {
    auto a = (unsigned int)std::round(this->a * 255.0f);
    auto b = (unsigned int)std::round(this->b * 255.0f);
    auto g = (unsigned int)std::round(this->g * 255.0f);
    auto r = (unsigned int)std::round(this->r * 255.0f);
    return ((a << 24) | (b << 16) | (g << 8) | r);
}

}  // namespace vox
