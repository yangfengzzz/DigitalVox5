//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.editor/editor_actions.h"
#include "vox.render/entity.h"

namespace vox::editor {
template <typename T>
inline Entity *EditorActions::CreateMonoComponentEntity(bool focus_on_creation, Entity *parent) {
    auto instance = CreateEmptyEntity(false, parent);

    T *component = instance->AddComponent<T>();

    instance->name = component->Name();

    if (focus_on_creation) SelectEntity(instance);

    return instance;
}

}  // namespace vox::editor
