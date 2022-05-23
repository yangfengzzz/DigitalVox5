//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/core/sampled_image.h"

#include "vox.render/rendering/render_target.h"

namespace vox::core {
SampledImage::SampledImage(const core::ImageView &image_view, Sampler *sampler)
    : image_view_{&image_view},
      target_attachment_{0},
      render_target_{nullptr},
      sampler_{sampler},
      is_depth_resolve_{false} {}

SampledImage::SampledImage(uint32_t target_attachment,
                           RenderTarget *render_target,
                           Sampler *sampler,
                           bool is_depth_resolve)
    : image_view_{nullptr},
      target_attachment_{target_attachment},
      render_target_{render_target},
      sampler_{sampler},
      is_depth_resolve_{is_depth_resolve} {}

SampledImage::SampledImage(const SampledImage &to_copy)
    : image_view_{to_copy.image_view_},
      target_attachment_{to_copy.target_attachment_},
      render_target_{to_copy.render_target_},
      sampler_{to_copy.sampler_},
      is_depth_resolve_{false} {}

SampledImage &SampledImage::operator=(const SampledImage &to_copy) {
    image_view_ = to_copy.image_view_;
    target_attachment_ = to_copy.target_attachment_;
    render_target_ = to_copy.render_target_;
    sampler_ = to_copy.sampler_;
    is_depth_resolve_ = to_copy.is_depth_resolve_;
    return *this;
}

SampledImage::SampledImage(SampledImage &&to_move) noexcept
    : image_view_{to_move.image_view_},
      target_attachment_{to_move.target_attachment_},
      render_target_{to_move.render_target_},
      sampler_{to_move.sampler_},
      is_depth_resolve_{to_move.is_depth_resolve_} {}

SampledImage &SampledImage::operator=(SampledImage &&to_move) noexcept {
    image_view_ = to_move.image_view_;
    target_attachment_ = to_move.target_attachment_;
    render_target_ = to_move.render_target_;
    sampler_ = to_move.sampler_;
    is_depth_resolve_ = to_move.is_depth_resolve_;
    return *this;
}

const core::ImageView &SampledImage::GetImageView(const RenderTarget &default_target) const {
    if (image_view_ != nullptr) {
        return *image_view_;
    } else {
        const auto &target = render_target_ ? *render_target_ : default_target;
        return target.GetViews().at(target_attachment_);
    }
}

const ImageView &SampledImage::GetImageView() const {
    if (image_view_ != nullptr) {
        return *image_view_;
    } else {
        throw std::runtime_error("provide default render target instead!.");
    }
}

const uint32_t *SampledImage::GetTargetAttachment() const {
    if (image_view_ != nullptr) {
        return nullptr;
    } else {
        return &target_attachment_;
    }
}

}  // namespace vox::core
