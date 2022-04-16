//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.


#ifndef DIGITALVOX_VOX_RENDER_IMAGE_MANAGER_H_
#define DIGITALVOX_VOX_RENDER_IMAGE_MANAGER_H_

#include "image.h"

namespace vox {
class ImageManager {
public:
    explicit ImageManager(Device& device);
    
    ~ImageManager() = default;
    
    /**
     * @brief Loads in a ktx 2D texture
     * @param file The filename of the texture to load
     */
    std::shared_ptr<Image> load_texture(const std::string &file);
    
    /**
     * @brief Loads in a ktx 2D texture array
     * @param file The filename of the texture to load
     */
    std::shared_ptr<Image> load_texture_array(const std::string &file);
    
    /**
     * @brief Loads in a ktx 2D texture cubemap
     * @param file The filename of the texture to load
     */
    std::shared_ptr<Image> load_texture_cubemap(const std::string &file);
    
    void collect_garbage();
    
private:
    Device& device_;
    std::vector<std::shared_ptr<Image>> image_pool_;
};

}
#endif /* DIGITALVOX_VOX_RENDER_IMAGE_MANAGER_H_ */
