//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader_property.h"
#include <utility>

namespace vox {
uint32_t ShaderProperty::property_name_counter = 0;

ShaderProperty::ShaderProperty(std::string name, ShaderDataGroup group) :
	name(std::move(name)),
	group(group),
	unique_id(ShaderProperty::property_name_counter) {
    ShaderProperty::property_name_counter += 1;
}

}
