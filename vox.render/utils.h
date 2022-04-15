//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "error.h"
#include "platform/filesystem.h"
#include "rendering/pipeline_state.h"
#include "rendering/render_context.h"

namespace vox {
/**
 * @brief Extracts the extension from an uri
 * @param uri An uniform Resource Identifier
 * @return The extension
 */
std::string get_extension(const std::string &uri);

/**
 * @param name String to convert to snake case
 * @return a snake case version of the string
 */
std::string to_snake_case(const std::string &name);

class CommandBuffer;

/**
 * @brief Takes a screenshot of the app by writing the swapchain image to file (slow function)
 * @param render_context The RenderContext to use
 * @param filename The name of the file to save the output to
 */
void screenshot(RenderContext &render_context, const std::string &filename);

}        // namespace vox
