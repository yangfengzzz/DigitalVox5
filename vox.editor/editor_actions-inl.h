//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "editor_actions.h"
#include "entity.h"

namespace vox::editor {
template<typename T>
inline Entity *EditorActions::create_mono_component_entity(bool focus_on_creation, Entity *parent) {
    auto instance = create_empty_entity(false, parent);
    
    T *component = instance->AddComponent<T>();
    
    instance->name_ = component->name();
    
    if (focus_on_creation)
        select_entity(instance);
    
    return instance;
}

}
