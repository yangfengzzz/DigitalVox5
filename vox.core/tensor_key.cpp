//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "tensor_key.h"

#include <memory>
#include <sstream>

#include "logging.h"
#include "tensor.h"

namespace vox {
namespace core {

class TensorKey::Impl {
public:
    Impl(TensorKeyMode mode) : mode_(mode) {}
    TensorKeyMode GetMode() const { return mode_; }
    virtual ~Impl() {}
    virtual std::string ToString() const = 0;

private:
    TensorKeyMode mode_;
};

class TensorKey::IndexImpl : public TensorKey::Impl {
public:
    IndexImpl(int64_t index) : TensorKey::Impl(TensorKeyMode::Index), index_(index) {}
    int64_t GetIndex() const { return index_; }
    std::string ToString() const override {
        std::stringstream ss;
        ss << "TensorKey::Index(" << index_ << ")";
        return ss.str();
    }

private:
    int64_t index_;
};

class TensorKey::SliceImpl : public TensorKey::Impl {
public:
    SliceImpl(std::optional<int64_t> start, std::optional<int64_t> stop, std::optional<int64_t> step)
        : TensorKey::Impl(TensorKeyMode::Slice), start_(start), stop_(stop), step_(step) {}
    std::shared_ptr<SliceImpl> InstantiateDimSize(int64_t dim_size) const {
        return std::make_shared<SliceImpl>(start_.has_value() ? start_.value() : 0,
                                           stop_.has_value() ? stop_.value() : dim_size,
                                           step_.has_value() ? step_.value() : 1);
    }
    
    int64_t GetStart() const {
        if (start_.has_value()) {
            return start_.value();
        } else {
            throw std::runtime_error("TensorKeyMode::Slice: start is None.");
        }
    }
    
    int64_t GetStop() const {
        if (stop_.has_value()) {
            return stop_.value();
        } else {
            throw std::runtime_error("TensorKeyMode::Slice: stop is None.");
        }
    }
    
    int64_t GetStep() const {
        if (step_.has_value()) {
            return step_.value();
        } else {
            throw std::runtime_error("TensorKeyMode::Slice: step is None.");
        }
    }
    
    std::string ToString() const override {
        std::stringstream ss;
        ss << "TensorKey::Slice(";
        if (start_.has_value()) {
            ss << start_.value();
        } else {
            ss << "None";
        }
        ss << ", ";
        if (stop_.has_value()) {
            ss << stop_.value();
        } else {
            ss << "None";
        }
        ss << ", ";
        if (step_.has_value()) {
            ss << step_.value();
        } else {
            ss << "None";
        }
        ss << ")";
        return ss.str();
    }

private:
    std::optional<int64_t> start_ = std::nullopt;
    std::optional<int64_t> stop_ = std::nullopt;
    std::optional<int64_t> step_ = std::nullopt;
};

class TensorKey::IndexTensorImpl : public TensorKey::Impl {
public:
    IndexTensorImpl(const Tensor& index_tensor)
        : TensorKey::Impl(TensorKeyMode::IndexTensor), index_tensor_(index_tensor) {}
    Tensor GetIndexTensor() const { return index_tensor_; }
    std::string ToString() const override {
        std::stringstream ss;
        ss << "TensorKey::IndexTensor(" << index_tensor_.ToString() << ")";
        return ss.str();
    }

private:
    Tensor index_tensor_;
};

TensorKey::TensorKey(const std::shared_ptr<Impl>& impl) : impl_(impl) {}

TensorKey::TensorKeyMode TensorKey::GetMode() const { return impl_->GetMode(); }

std::string TensorKey::ToString() const { return impl_->ToString(); }

TensorKey TensorKey::Index(int64_t index) { return TensorKey(std::make_shared<IndexImpl>(index)); }

TensorKey TensorKey::Slice(std::optional<int64_t> start, std::optional<int64_t> stop, std::optional<int64_t> step) {
    return TensorKey(std::make_shared<SliceImpl>(start, stop, step));
}

TensorKey TensorKey::IndexTensor(const Tensor& index_tensor) {
    return TensorKey(std::make_shared<IndexTensorImpl>(index_tensor));
}

int64_t TensorKey::GetIndex() const {
    if (auto index_impl = std::dynamic_pointer_cast<IndexImpl>(impl_)) {
        return index_impl->GetIndex();
    } else {
        throw std::runtime_error("GetIndex() failed: the impl is not IndexImpl.");
    }
}

int64_t TensorKey::GetStart() const {
    if (auto slice_impl = std::dynamic_pointer_cast<SliceImpl>(impl_)) {
        return slice_impl->GetStart();
    } else {
        throw std::runtime_error("GetStart() failed: the impl is not SliceImpl.");
    }
}

int64_t TensorKey::GetStop() const {
    if (auto slice_impl = std::dynamic_pointer_cast<SliceImpl>(impl_)) {
        return slice_impl->GetStop();
    } else {
        throw std::runtime_error("GetStop() failed: the impl is not SliceImpl.");
    }
}

int64_t TensorKey::GetStep() const {
    if (auto slice_impl = std::dynamic_pointer_cast<SliceImpl>(impl_)) {
        return slice_impl->GetStep();
    } else {
        throw std::runtime_error("GetStep() failed: the impl is not SliceImpl.");
    }
}

TensorKey TensorKey::InstantiateDimSize(int64_t dim_size) const {
    if (auto slice_impl = std::dynamic_pointer_cast<SliceImpl>(impl_)) {
        return TensorKey(slice_impl->InstantiateDimSize(dim_size));
    } else {
        throw std::runtime_error("InstantiateDimSize() failed: the impl is not SliceImpl.");
    }
}

Tensor TensorKey::GetIndexTensor() const {
    if (auto index_tensor_impl = std::dynamic_pointer_cast<IndexTensorImpl>(impl_)) {
        return index_tensor_impl->GetIndexTensor();
    } else {
        throw std::runtime_error("GetIndexTensor() failed: the impl is not IndexTensorImpl.");
    }
}

}  // namespace core
}  // namespace vox
