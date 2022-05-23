//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/core/sampled_image.h"
#include "vox.render/rendering/postprocessing_pass.h"
#include "vox.render/rendering/render_pipeline.h"
#include "vox.render/rendering/subpass.h"

namespace vox {
/**
 * @brief An utility struct for hashing pairs.
 */
struct PairHasher {
    template <typename TPair>
    size_t operator()(const TPair &pair) const {
        std::hash<decltype(pair.first)> hash1{};
        return hash1(pair.first) * 43 + pair.second;
    }
};

/**
 * @brief Maps in-shader binding names to indices into a RenderTarget's attachments.
 */
using AttachmentMap = std::unordered_map<std::string, uint32_t>;

/**
 * @brief Maps in-shader binding names to the core::SampledImage to bind.
 */
using SampledMap = std::unordered_map<std::string, core::SampledImage>;

/**
 * @brief Maps in-shader binding names to the core::ImageView to bind for storage images.
 */
using StorageImageMap = std::unordered_map<std::string, const core::ImageView *>;

/**
 * @brief A list of indices into a RenderTarget's attachments.
 */
using AttachmentList = std::vector<uint32_t>;

/**
 * @brief A set of indices into a RenderTarget's attachments.
 */
using AttachmentSet = std::unordered_set<uint32_t>;

class PostProcessingRenderPass;

/**
 * @brief A single step of a vox::PostProcessingRenderPass.
 */
class PostProcessingSubpass : public Subpass {
public:
    PostProcessingSubpass(PostProcessingRenderPass *parent,
                          RenderContext &render_context,
                          std::shared_ptr<ShaderSource> triangle_vs,
                          std::shared_ptr<ShaderSource> fs,
                          ShaderVariant &&fs_variant = {});

    PostProcessingSubpass(const PostProcessingSubpass &to_copy) = delete;
    PostProcessingSubpass &operator=(const PostProcessingSubpass &to_copy) = delete;

    PostProcessingSubpass(PostProcessingSubpass &&to_move) noexcept;
    PostProcessingSubpass &operator=(PostProcessingSubpass &&to_move) = delete;

    ~PostProcessingSubpass() override = default;

    /**
     * @brief Maps the names of input attachments in the shader to indices into the render target's images.
     *        These are given as `subpassInput`s to the subpass, at set 0; they are bound automatically according to
     * their name.
     */
    [[nodiscard]] inline const AttachmentMap &GetInputAttachments() const { return input_attachments_; }

    /**
     * @brief Maps the names of samplers in the shader to vox::core::SampledImage.
     *        These are given as samplers to the subpass, at set 0; they are bound automatically according to their
     * name.
     * @remarks PostProcessingPipeline::GetSampler() is used as the default sampler if none is specified.
     *          The RenderTarget for the current PostProcessingSubpass is used if none is specified for attachment
     * images.
     */
    [[nodiscard]] inline const SampledMap &GetSampledImages() const { return sampled_images_; }

    /**
     * @brief Maps the names of storage images in the shader to vox::core::ImageView.
     *        These are given as image2D[Array] to the subpass, at set 0; they are bound automatically according to
     * their name.
     */
    [[nodiscard]] inline const StorageImageMap &GetStorageImages() const { return storage_images_; }

    /**
     * @brief Returns the shader variant used for this postprocess' fragment shader.
     */
    inline ShaderVariant &GetFsVariant() { return fs_variant_; }

    /**
     * @brief Sets the shader variant that will be used for this postprocess' fragment shader.
     */
    inline PostProcessingSubpass &SetFsVariant(ShaderVariant &&new_variant) {
        fs_variant_ = std::move(new_variant);

        return *this;
    }

    /**
     * @brief Changes the debug name of this Subpass.
     */
    inline PostProcessingSubpass &SetDebugName(const std::string &name) {
        Subpass::SetDebugName(name);

        return *this;
    }

    /**
     * @brief Changes (or adds) the input attachment at name for this step.
     */
    PostProcessingSubpass &BindInputAttachment(const std::string &name, uint32_t new_input_attachment);

    /**
     * @brief Changes (or adds) the sampled image at name for this step.
     * @remarks If no RenderTarget is specifically set for the core::SampledImage,
     *          it will default to sample in the RenderTarget currently bound for drawing in the parent
     * PostProcessingRenderpass.
     */
    PostProcessingSubpass &BindSampledImage(const std::string &name, core::SampledImage &&new_image);

    /**
     * @brief Changes (or adds) the storage image at name for this step.
     */
    PostProcessingSubpass &BindStorageImage(const std::string &name, const core::ImageView &new_image);

    /**
     * @brief Set the constants that are pushed before each fullscreen draw.
     */
    PostProcessingSubpass &SetPushConstants(const std::vector<uint8_t> &data);

    /**
     * @brief Set the constants that are pushed before each fullscreen draw.
     */
    template <typename T>
    inline PostProcessingSubpass &SetPushConstants(const T &data) {
        push_constants_data_.reserve(sizeof(data));
        auto data_ptr = reinterpret_cast<const uint8_t *>(&data);
        push_constants_data_.assign(data_ptr, data_ptr + sizeof(data));

        return *this;
    }

    /**
     * @brief A functor used to draw the primitives for a post-processing step.
     * @see DefaultDrawFunc()
     */
    using DrawFunc = std::function<void(CommandBuffer &command_buffer, RenderTarget &render_target)>;

    /**
     * @brief Sets the function used to draw this postprocessing step.
     * @see DefaultDrawFunc()
     */
    PostProcessingSubpass &SetDrawFunc(DrawFunc &&new_func);

    /**
     * @brief The default function used to draw a step; it draws 1 instance with 3 vertices.
     */
    static void DefaultDrawFunc(vox::CommandBuffer &command_buffer, vox::RenderTarget &render_target);

private:
    PostProcessingRenderPass *parent_;

    std::shared_ptr<ShaderSource> vertex_shader_{nullptr};
    std::shared_ptr<ShaderSource> fragment_shader_{nullptr};
    ShaderVariant fs_variant_{};

    AttachmentMap input_attachments_{};
    SampledMap sampled_images_{};
    StorageImageMap storage_images_{};

    std::vector<uint8_t> push_constants_data_{};

    DrawFunc draw_func_{&PostProcessingSubpass::DefaultDrawFunc};

    void Prepare() override;
    void Draw(CommandBuffer &command_buffer) override;
};

// MARK: - PostProcessingRenderPass
/**
 * @brief A collection of vox::PostProcessingSubpass that are run as a single renderpass.
 */
class PostProcessingRenderPass : public PostProcessingPass<PostProcessingRenderPass> {
public:
    friend class PostProcessingSubpass;

    explicit PostProcessingRenderPass(PostProcessingPipeline *parent,
                                      std::unique_ptr<core::Sampler> &&default_sampler = nullptr);

    PostProcessingRenderPass(const PostProcessingRenderPass &to_copy) = delete;
    PostProcessingRenderPass &operator=(const PostProcessingRenderPass &to_copy) = delete;

    PostProcessingRenderPass(PostProcessingRenderPass &&to_move) = default;
    PostProcessingRenderPass &operator=(PostProcessingRenderPass &&to_move) = default;

    void Draw(CommandBuffer &command_buffer, RenderTarget &default_render_target) override;

    /**
     * @brief Gets the step at the given index.
     */
    inline PostProcessingSubpass &GetSubpass(size_t index) {
        return *dynamic_cast<PostProcessingSubpass *>(pipeline_.GetSubpasses().at(index).get());
    }

    /**
     * @brief Constructs a new PostProcessingSubpass and adds it to the tail of the pipeline.
     * @remarks `this`, the render context and the vertex shader source are passed automatically before `args`.
     * @returns The inserted step.
     */
    template <typename... ConstructorArgs>
    PostProcessingSubpass &AddSubpass(ConstructorArgs &&...args) {
        ShaderSource vs_copy = GetTriangleVs();
        auto new_subpass = std::make_unique<PostProcessingSubpass>(this, GetRenderContext(), std::move(vs_copy),
                                                                   std::forward<ConstructorArgs>(args)...);
        auto &new_subpass_ref = *new_subpass;

        pipeline_.AddSubpass(std::move(new_subpass));

        return new_subpass_ref;
    }

    /**
     * @brief Set the uniform data to be bound at set 0, binding 0.
     */
    template <typename T>
    inline PostProcessingRenderPass &SetUniformData(const T &data) {
        uniform_data_.reserve(sizeof(data));
        auto data_ptr = reinterpret_cast<const uint8_t *>(&data);
        uniform_data_.assign(data_ptr, data_ptr + sizeof(data));

        return *this;
    }

    /**
     * @copydoc SetUniformData(const T&)
     */
    inline PostProcessingRenderPass &SetUniformData(const std::vector<uint8_t> &data) {
        uniform_data_ = data;

        return *this;
    }

private:
    // An attachment sampled from a rendertarget
    using SampledAttachmentSet = std::unordered_set<std::pair<RenderTarget *, uint32_t>, PairHasher>;

    /**
     * @brief Transition input, sampled and output attachments as appropriate.
     * @remarks If a RenderTarget is not explicitly set for this pass, fallback_render_target is used.
     */
    void TransitionAttachments(const AttachmentSet &input_attachments,
                               const SampledAttachmentSet &sampled_attachments,
                               const AttachmentSet &output_attachments,
                               CommandBuffer &command_buffer,
                               RenderTarget &fallback_render_target);

    /**
     * @brief Select appropriate load/store operations for each buffer of render_target,
     *        according to the subpass inputs/sampled inputs/subpass outputs of all steps
     *        in the pipeline.
     * @remarks If a RenderTarget is not explicitly set for this pass, fallback_render_target is used.
     */
    void UpdateLoadStores(const AttachmentSet &input_attachments,
                          const SampledAttachmentSet &sampled_attachments,
                          const AttachmentSet &output_attachments,
                          const RenderTarget &fallback_render_target);

    /**
     * @brief Transition images and prepare load/stores before draw()ing.
     */
    void PrepareDraw(CommandBuffer &command_buffer, RenderTarget &fallback_render_target);

    [[nodiscard]] BarrierInfo GetSrcBarrierInfo() const override;
    [[nodiscard]] BarrierInfo GetDstBarrierInfo() const override;

    RenderPipeline pipeline_{};
    std::unique_ptr<core::Sampler> default_sampler_{};
    RenderTarget *draw_render_target_{nullptr};
    std::vector<LoadStoreInfo> load_stores_{};
    bool load_stores_dirty_{true};
    std::vector<uint8_t> uniform_data_{};
    std::shared_ptr<BufferAllocation> uniform_buffer_alloc_{};
};

}  // namespace vox
