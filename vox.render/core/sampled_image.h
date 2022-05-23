//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <memory>

#include "vox.render/core/image.h"
#include "vox.render/core/sampler.h"

namespace vox {
class RenderTarget;

namespace core {
/**
 * @brief A reference to a vox::core::ImageView, plus an optional sampler for it
 *        - either coming from a vox::RenderTarget or from a user-created Image.
 */
class SampledImage {
public:
    /**
     * @brief Constructs a SampledImage referencing the given image and with the given sampler.
     * @remarks If the sampler is null, a default sampler will be used.
     */
    explicit SampledImage(const ImageView &image_view, Sampler *sampler = nullptr);

    /**
     * @brief Constructs a SampledImage referencing a certain attachment of a render target.
     * @remarks If the render target is null, the default is assumed.
     *          If the sampler is null, a default sampler is used.
     */
    explicit SampledImage(uint32_t target_attachment,
                          RenderTarget *render_target = nullptr,
                          Sampler *sampler = nullptr,
                          bool is_depth_resolve = false);

    SampledImage(const SampledImage &to_copy);

    SampledImage &operator=(const SampledImage &to_copy);

    SampledImage(SampledImage &&to_move) noexcept;

    SampledImage &operator=(SampledImage &&to_move) noexcept;

    ~SampledImage() = default;

    /**
     * @brief Replaces the current image view with the given one.
     */
    inline void SetImageView(const ImageView &new_view) { image_view_ = &new_view; }

    /**
     * @brief Replaces the image view with an attachment of the PostProcessingPipeline's render target.
     */
    inline void SetImageView(uint32_t new_attachment) {
        image_view_ = nullptr;
        target_attachment_ = new_attachment;
    }

    /**
     * @brief If this view refers to a render target attachment, returns a pointer to its index;
     *        otherwise, returns `null`.
     * @remarks The lifetime of the returned pointer matches that of this `SampledImage`.
     */
    [[nodiscard]] const uint32_t *GetTargetAttachment() const;

    /**
     * @brief Returns either the ImageView, if set, or the image view for the set target attachment.
     *        If the view has no render target associated with it, default_target is used.
     */
    [[nodiscard]] const ImageView &GetImageView(const vox::RenderTarget &default_target) const;

    [[nodiscard]] const ImageView &GetImageView() const;

    /**
     * @brief Returns the currently-set sampler, if any.
     */
    [[nodiscard]] inline Sampler *GetSampler() const { return sampler_; }

    /**
     * @brief Sets the sampler for this SampledImage.
     */
    inline void SetSampler(Sampler *new_sampler) { sampler_ = new_sampler; }

    /**
     * @brief Returns the RenderTarget, if set.
     */
    [[nodiscard]] inline RenderTarget *GetRenderTarget() const { return render_target_; }

    /**
     * @brief Returns either the RenderTarget, if set, or - if not - the given fallback render target.
     */
    inline RenderTarget &GetRenderTarget(RenderTarget &fallback) const {
        return render_target_ ? *render_target_ : fallback;
    }

    /**
     * @brief Sets the sampler for this SampledImage.
     *        Setting it to null will make it use the default instead.
     */
    inline void SetRenderTarget(RenderTarget *new_render_target) { render_target_ = new_render_target; }

    [[nodiscard]] inline bool IsDepthResolve() const { return is_depth_resolve_; }

private:
    const ImageView *image_view_;
    uint32_t target_attachment_;
    RenderTarget *render_target_;
    Sampler *sampler_;
    bool is_depth_resolve_;
};

}  // namespace core
}  // namespace vox
