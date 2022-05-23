//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"

namespace vox {
class PbrApp : public ForwardApplication {
public:
    void LoadScene() override;

private:
    struct Material {
        std::string name;
        Color base_color;
        float roughness;
        float metallic;

        Material(){};

        Material(std::string n, Color c, float r, float m) : name(n) {
            roughness = r;
            metallic = m;
            base_color = c;
        };
    };
    std::vector<Material> materials_;
};

}  // namespace vox
