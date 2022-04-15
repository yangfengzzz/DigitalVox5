//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <ktx.h>
#include <string>

namespace vox {
/**
 * @brief Class representing a heightmap loaded from a ktx texture
 */
class HeightMap {
public:
    /**
     * @brief Loads in a ktx texture as a heightmap
     * @param filename The ktx file to load
     * @param patchsize The patch size
     */
    HeightMap(const std::string &filename, uint32_t patchsize);
    
    ~HeightMap();
    
    /**
     * @brief Retrieves a value from the heightmap at a specific coordinates
     * @param x The x coordinate
     * @param y The y coordinate
     * @returns A float height value
     */
    float get_height(uint32_t x, uint32_t y);
    
private:
    uint16_t *data_;
    
    uint32_t dim_;
    
    uint32_t scale_;
};

}        // namespace vox
