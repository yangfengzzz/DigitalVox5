//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <functional>

#include "vox.render/entity.h"
#include "vox.render/ui/widgets/menu/menu_list.h"

namespace vox::editor {
/**
 * Class exposing tools to generate an entity creation menu
 */
class EntityCreationMenu {
public:
    /**
     * Generates an entity creation menu under the given MenuList item.
     * Also handles custom additional OnClick callback
     */
    static void GenerateEntityCreationMenu(::vox::ui::MenuList &menu_list,
                                           Entity *parent = nullptr,
                                           const std::optional<std::function<void()>> &on_item_clicked = {});
};

}  // namespace vox::editor
