//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/ijson_convertible.h"
#include "vox.math/color.h"

namespace vox::visualization {

/// \class RenderOption
///
/// \brief Defines rendering options for visualizer.
class RenderOption : public utility::IJsonConvertible {
public:
    // Global options
    enum class TextureInterpolationOption {
        Nearest = 0,
        Linear = 1,
    };

    enum class DepthFunc {
        Never = 0,
        Less = 1,
        Equal = 2,
        LEqual = 3,
        Greater = 4,
        NotEqual = 5,
        GEqual = 6,
        Always = 7
    };

    /// \enum PointColorOption
    ///
    /// \brief Enum class for point color for PointCloud.
    enum class PointColorOption {
        Default = 0,
        Color = 1,
        XCoordinate = 2,
        YCoordinate = 3,
        ZCoordinate = 4,
        Normal = 9,
    };

    const double POINT_SIZE_MAX = 25.0;
    const double POINT_SIZE_MIN = 1.0;
    const double POINT_SIZE_STEP = 1.0;
    const double POINT_SIZE_DEFAULT = 5.0;
    const double LINE_WIDTH_MAX = 10.0;
    const double LINE_WIDTH_MIN = 1.0;
    const double LINE_WIDTH_STEP = 1.0;
    const double LINE_WIDTH_DEFAULT = 1.0;

    /// \enum MeshShadeOption
    ///
    /// \brief Enum class for mesh shading for TriangleMesh.
    enum class MeshShadeOption {
        FlatShade = 0,
        SmoothShade = 1,
    };

    /// \enum MeshColorOption
    ///
    /// \brief Enum class for color for TriangleMesh.
    enum class MeshColorOption {
        Default = 0,
        Color = 1,
        XCoordinate = 2,
        YCoordinate = 3,
        ZCoordinate = 4,
        Normal = 9,
    };

    // Image options
    enum class ImageStretchOption {
        OriginalSize = 0,
        StretchKeepRatio = 1,
        StretchWithWindow = 2,
    };

public:
    /// \brief Default Constructor.
    RenderOption() = default;
    ~RenderOption() override = default;

public:
    bool ConvertToJsonValue(Json::Value &value) const override;
    bool ConvertFromJsonValue(const Json::Value &value) override;

public:
    void ChangePointSize(double change);
    void SetPointSize(double size);
    void ChangeLineWidth(double change);
    void TogglePointShowNormal() { point_show_normal_ = !point_show_normal_; }
    void ToggleShadingOption() {
        if (mesh_shade_option_ == MeshShadeOption::FlatShade) {
            mesh_shade_option_ = MeshShadeOption::SmoothShade;
        } else {
            mesh_shade_option_ = MeshShadeOption::FlatShade;
        }
    }
    void ToggleMeshShowBackFace() { mesh_show_back_face_ = !mesh_show_back_face_; }
    void ToggleMeshShowWireframe() { mesh_show_wireframe_ = !mesh_show_wireframe_; }
    void ToggleImageStretchOption() {
        if (image_stretch_option_ == ImageStretchOption::OriginalSize) {
            image_stretch_option_ = ImageStretchOption::StretchKeepRatio;
        } else if (image_stretch_option_ == ImageStretchOption::StretchKeepRatio) {
            image_stretch_option_ = ImageStretchOption::StretchWithWindow;
        } else {
            image_stretch_option_ = ImageStretchOption::OriginalSize;
        }
    }

public:
    // PointCloud options
    /// Point size for PointCloud.
    double point_size_ = POINT_SIZE_DEFAULT;
    /// Point color option for PointCloud.
    PointColorOption point_color_option_ = PointColorOption::Default;
    /// Whether to show normal for PointCloud.
    bool point_show_normal_ = false;

    // TriangleMesh options
    /// Mesh shading option for TriangleMesh.
    MeshShadeOption mesh_shade_option_ = MeshShadeOption::FlatShade;
    /// Color option for TriangleMesh.
    MeshColorOption mesh_color_option_ = MeshColorOption::Color;
    /// Whether to show back faces for TriangleMesh.
    bool mesh_show_back_face_ = false;

    bool mesh_show_wireframe_ = false;
    Color default_mesh_color_ = Color(0.7, 0.7, 0.7);

    // LineSet options
    /// Line width for LineSet.
    double line_width_ = LINE_WIDTH_DEFAULT;

    // Image options
    ImageStretchOption image_stretch_option_ = ImageStretchOption::StretchKeepRatio;
    int image_max_depth_ = 3000;

    // Coordinate frame
    /// Whether to show coordinate frame.
    bool show_coordinate_frame_ = false;
};

}  // namespace vox::visualization
