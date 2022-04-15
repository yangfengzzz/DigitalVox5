//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "platform/platform.h"

namespace vox {
enum UnixType {
    MAC,
    LINUX
};

class UnixPlatform : public Platform {
public:
    UnixPlatform(const UnixType &type, int argc, char **argv);
    
    ~UnixPlatform() override = default;
    
    const char *get_surface_extension() override;
    
protected:
    void create_window(const Window::Properties &properties) override;
    
private:
    UnixType type_;
};

}        // namespace vox
