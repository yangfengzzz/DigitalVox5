//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.


#ifndef DIGITALVOX_VOX_RENDER_IMAGE_MANAGER_H_
#define DIGITALVOX_VOX_RENDER_IMAGE_MANAGER_H_

#include "image.h"
#include "singleton.h"
#include "rendering/postprocessing_pipeline.h"
#include "rendering/postprocessing_computepass.h"

namespace vox {
class ImageManager: public Singleton<ImageManager> {
public:
    static ImageManager &get_singleton();
    
    static ImageManager *get_singleton_ptr();
    
    explicit ImageManager(Device& device);
    
    ~ImageManager() = default;
    
    void collect_garbage();

public:
    /**
     * @brief Loads in a ktx 2D texture
     */
    std::shared_ptr<Image> load_texture(const std::string &file);
    
    /**
     * @brief Loads in a ktx 2D texture array
     */
    std::shared_ptr<Image> load_texture_array(const std::string &file);
    
    /**
     * @brief Loads in a ktx 2D texture cubemap
     */
    std::shared_ptr<Image> load_texture_cubemap(const std::string &file);
    
public:
    std::shared_ptr<Image> generate_ibl(const std::string &file,
                                        CommandBuffer &command_buffer,
                                        RenderContext &render_context);
    
private:
    Device& device_;
    std::unordered_map<std::string, std::shared_ptr<Image>> image_pool_;
    
    ShaderData shader_data_;
    std::unique_ptr<PostProcessingPipeline> pipeline_{nullptr};
    PostProcessingComputePass* ibl_pass_{nullptr};
};

template<> inline ImageManager *Singleton<ImageManager>::ms_singleton_{nullptr};

}
#endif /* DIGITALVOX_VOX_RENDER_IMAGE_MANAGER_H_ */
