//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/event.h"

namespace vox::editor {
/**
 * Accessible from anywhere editor settings
 */
class EditorSettings {
public:
    template <typename T>
    class Property {
    public:
        /**
         * Creates the property with a default value
         */
        Property(T value) : value_(value) {}

        /**
         * Event called when the property value changes
         */
        Event<T> on_value_changed_;

        /**
         * Assign a new value to the property
         */
        inline T &operator=(T value) {
            Set(value);
            return value_;
        }

        /**
         * Assign a new value to the property
         */
        inline void Set(T value) {
            value_ = value;
            on_value_changed_.Invoke(value_);
        }

        inline operator T() { return value_; }

        /**
         * Returns the value of the property
         */
        [[nodiscard]] inline T Get() const { return value_; }

    private:
        T value_;
    };

    /**
     * No construction possible
     */
    EditorSettings() = delete;

    inline static Property<bool> show_geometry_bounds_ = {false};
    inline static Property<bool> show_light_bounds_ = {false};
    inline static Property<bool> show_geometry_frustum_culling_in_scene_view_ = {false};
    inline static Property<bool> show_light_frustum_culling_in_scene_view_ = {false};
    inline static Property<float> light_billboard_scale_ = {0.5f};
    inline static Property<float> translation_snap_unit_ = {1.0f};
    inline static Property<float> rotation_snap_unit_ = {15.0f};
    inline static Property<float> scaling_snap_unit_ = {1.0f};
};

}  // namespace vox::editor
