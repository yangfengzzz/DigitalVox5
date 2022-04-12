/* Copyright (c) 2018-2022, Arm Limited and Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 the "License";
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "platform/window.h"

namespace vox {
/**
 * @brief Surface-less implementation of a Window, for use in headless rendering
 */
class HeadlessWindow : public Window {
public:
    HeadlessWindow(const Window::Properties &properties);
    
    virtual ~HeadlessWindow() = default;
    
    /**
     * @brief A direct window doesn't have a surface
     * @returns VK_NULL_HANDLE
     */
    VkSurfaceKHR create_surface(Instance &instance) override;
    
    /**
     * @brief A direct window doesn't have a surface
     * @returns nullptr
     */
    VkSurfaceKHR create_surface(VkInstance instance, VkPhysicalDevice physical_device) override;
    
    bool should_close() override;
    
    void close() override;
    
    float get_dpi_factor() const override;
    
private:
    bool closed{false};
};

}        // namespace vox
