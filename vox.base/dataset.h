//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace vox::data {

/// Function to return default data root directory in the following order:
///
/// (a) OPEN3D_DATA_ROOT environment variable.
/// (b) $HOME/open3d_data.
std::string LocateDataRoot();

/// \class Dataset
/// \brief Base Open3D dataset class.
///
/// The Dataset classes in Open3D are designed for convenient access to
/// "built-in" example and test data. You'll need internet access to use the
/// dataset classes. The downloaded data will be stored in the Open3D's data
/// root directory.
///
/// - A dataset class locates the data root directory in the following order:
///   (a) User-specified by `data_root` when instantiating a dataset object.
///   (b) OPEN3D_DATA_ROOT environment variable.
///   (c) $HOME/open3d_data.
///   By default, (c) will be used, and it is also the recommended way.
/// - When a dataset object is instantiated, the corresponding data will be
///   downloaded in `${data_root}/download/prefix/` and extracted or copied to
///   `${data_root}/extract/prefix/`. If the extracted data directory exists,
///   the files will be used without validation. If it does not exists, and the
///   valid downloaded file exists, the data will be extracted from the
///   downloaded file. If downloaded file does not exists, or validates against
///   the provided MD5, it will be re-downloaded.
/// - After the data is downloaded and extracted, the dataset object will NOT
///   load the data for you. Instead, you will get the paths to the data files
///   and use Open3D's I/O functions to load the data. This design exposes where
///   the data is stored and how the data is loaded, allowing users to modify
///   the code and load their own data in a similar way. Please check the
///   documentation of the specific dataset to know more about the specific
///   functionalities provided for it.
class Dataset {
public:
    /// \brief Parameterized Constructor.
    ///
    /// \param prefix Prefix of the dataset. The data is downloaded in
    /// `${data_root}/download/${prefix}/` and extracted in
    /// `${data_root}/extract/${prefix}/`.
    /// \param data_root Path to `${data_root}`, which contains all the
    /// downloaded and extracted files.
    /// The data root directory is located in the following order:
    ///   (a) User-specified by `data_root` when instantiating a dataset object.
    ///   (b) OPEN3D_DATA_ROOT environment variable.
    ///   (c) $HOME/open3d_data.
    ///   By default, (c) will be used, and it is also the recommended way.
    explicit Dataset(std::string prefix, const std::string& data_root = "");

    virtual ~Dataset() = default;

    /// \brief Get data root directory. The data root is set at construction
    /// time or automatically determined.
    [[nodiscard]] std::string GetDataRoot() const { return data_root_; }
    /// \brief Get prefix for the dataset.
    [[nodiscard]] std::string GetPrefix() const { return prefix_; }

    /// \brief Get absolute path to download directory. i.e.
    /// ${data_root}/${download_prefix}/${prefix}
    [[nodiscard]] std::string GetDownloadDir() const { return GetDataRoot() + "/download/" + GetPrefix(); }
    /// \brief Get absolute path to extract directory. i.e.
    /// ${data_root}/${extract_prefix}/${prefix}
    [[nodiscard]] std::string GetExtractDir() const { return GetDataRoot() + "/extract/" + GetPrefix(); }

protected:
    /// Open3D data root.
    std::string data_root_;
    /// Dataset prefix.
    std::string prefix_;
};

/// \class SingleDownloadDataset
/// \brief This class allows user to create simple dataset which includes single
/// file downloading and extracting / copying.
class SingleDownloadDataset : public Dataset {
public:
    SingleDownloadDataset(const std::string& prefix,
                          const std::vector<std::string>& urls,
                          const std::string& md5,
                          bool no_extract = false,
                          const std::string& data_root = "");

    ~SingleDownloadDataset() override = default;
};

/// \class DemoICPPointClouds
/// \brief Data class for `DemoICPPointClouds` contains 3 point clouds of binary
/// PCD format. This data is used in Open3D for ICP demo.
/// \copyright Creative Commons 3.0 (CC BY 3.0).
class DemoICPPointClouds : public SingleDownloadDataset {
public:
    explicit DemoICPPointClouds(const std::string& data_root = "");

    /// \brief Returns list of 3 point cloud paths.
    [[nodiscard]] std::vector<std::string> GetPaths() const { return paths_; }
    /// \brief Returns path to the point cloud at index. Use `GetPaths(0)`,
    /// `GetPaths(1)`, and `GetPaths(2)` to access the paths.
    [[nodiscard]] std::string GetPaths(size_t index) const;
    /// \brief Returns path to the transformation metadata log file, containing
    /// transformation between frame 0 and 1, and frame 1 and 2.
    [[nodiscard]] std::string GetTransformationLogPath() const { return transformation_log_path_; }

private:
    // List of path to PCD point-cloud fragments.
    std::vector<std::string> paths_;
    std::string transformation_log_path_;
};

/// \class DemoColoredICPPointClouds
/// \brief Data class for `DemoColoredICPPointClouds` contains 2
/// point clouds of PLY format. This data is used in Open3D for
/// Colored-ICP demo.
/// \copyright Creative Commons 3.0 (CC BY 3.0).
class DemoColoredICPPointClouds : public SingleDownloadDataset {
public:
    explicit DemoColoredICPPointClouds(const std::string& data_root = "");

    /// \brief Returns list of list of 2 point cloud paths.
    [[nodiscard]] std::vector<std::string> GetPaths() const { return paths_; }
    /// \brief Returns path to the point cloud at index. Use `GetPaths(0)`, and
    /// `GetPaths(1)` to access the paths.
    [[nodiscard]] std::string GetPaths(size_t index) const;

private:
    // List of path to PCD point-cloud fragments.
    std::vector<std::string> paths_;
};

/// \class DemoCropPointCloud
/// \brief Data class for `DemoCropPointCloud` contains a point cloud, and
/// `cropped.json` (a saved selected polygon volume file). This data is used
/// in Open3D for point cloud crop demo.
/// \copyright Creative Commons 3.0 (CC BY 3.0).
class DemoCropPointCloud : public SingleDownloadDataset {
public:
    explicit DemoCropPointCloud(const std::string& data_root = "");

    /// \brief Returns path to example point cloud.
    [[nodiscard]] std::string GetPointCloudPath() const { return point_cloud_path_; }
    /// \brief Returns path to saved selected polygon volume file.
    [[nodiscard]] std::string GetCroppedJSONPath() const { return cropped_json_path_; }

private:
    // Path to example point cloud.
    std::string point_cloud_path_;
    // Path to saved selected polygon volume file.
    std::string cropped_json_path_;
};

/// \class DemoFeatureMatchingPointClouds
/// \brief Data class for `DemoFeatureMatchingPointClouds` contains 2
/// pointcloud fragments and their respective FPFH features and L32D features.
/// This data is used in Open3D for point cloud feature matching demo.
/// \copyright Creative Commons 3.0 (CC BY 3.0).
class DemoFeatureMatchingPointClouds : public SingleDownloadDataset {
public:
    explicit DemoFeatureMatchingPointClouds(const std::string& data_root = "");

    /// \brief Returns list of paths to point clouds, of size 2.
    [[nodiscard]] std::vector<std::string> GetPointCloudPaths() const { return point_cloud_paths_; }
    /// \brief Returns list of paths to saved FPFH features binary for point
    /// clouds, respectively, of size 2.
    [[nodiscard]] std::vector<std::string> GetFPFHFeaturePaths() const { return fpfh_feature_paths_; }
    /// \brief Returns list of paths to saved L32D features binary for point
    /// clouds, respectively, of size 2.
    [[nodiscard]] std::vector<std::string> GetL32DFeaturePaths() const { return l32d_feature_paths_; }

private:
    /// List of paths to point clouds, of size 2.
    std::vector<std::string> point_cloud_paths_;
    /// List of saved FPFH features binary for point clouds,
    /// respectively, of size 2.
    std::vector<std::string> fpfh_feature_paths_;
    /// List of saved L32D features binary for point clouds,
    /// respectively, of size 2.
    std::vector<std::string> l32d_feature_paths_;
};

/// \class DemoPoseGraphOptimization
/// \brief Data class for `DemoPoseGraphOptimization` contains an example
/// fragment pose graph, and global pose graph. This data is used in Open3D
/// for pose graph optimization demo.
class DemoPoseGraphOptimization : public SingleDownloadDataset {
public:
    explicit DemoPoseGraphOptimization(const std::string& data_root = "");

    /// \brief Returns path to example global pose graph (json).
    [[nodiscard]] std::string GetPoseGraphFragmentPath() const { return pose_graph_fragment_path_; }
    /// \brief Returns path to example fragment pose graph (json).
    [[nodiscard]] std::string GetPoseGraphGlobalPath() const { return pose_graph_global_path_; }

private:
    /// Path to example global pose graph (json).
    std::string pose_graph_fragment_path_;
    /// Path to example fragment pose graph (json).
    std::string pose_graph_global_path_;
};

/// \class DemoCustomVisualization
/// \brief Data class for `DemoCustomVisualization` contains an example
/// point-cloud, camera trajectory (json file), rendering options (json file).
/// This data is used in Open3D for custom visualization with camera trajectory
/// demo.
class DemoCustomVisualization : public SingleDownloadDataset {
public:
    explicit DemoCustomVisualization(const std::string& data_root = "");

    /// \brief Returns path to the point cloud (ply).
    [[nodiscard]] std::string GetPointCloudPath() const { return point_cloud_path_; }
    /// \brief Returns path to the camera_trajectory.json.
    [[nodiscard]] std::string GetCameraTrajectoryPath() const { return camera_trajectory_path_; }
    /// \brief Returns path to the renderoption.json.
    [[nodiscard]] std::string GetRenderOptionPath() const { return render_option_path_; }

private:
    std::string point_cloud_path_;
    std::string camera_trajectory_path_;
    std::string render_option_path_;
};

/// \class PCDPointCloud
/// \brief Data class for `PCDPointCloud` contains the `fragment.pcd`
/// point cloud mesh from the `Redwood Living Room` dataset.
class PCDPointCloud : public SingleDownloadDataset {
public:
    explicit PCDPointCloud(const std::string& data_root = "");

    /// \brief Returns path to the `pcd` format point cloud.
    [[nodiscard]] std::string GetPath() const { return path_; };

private:
    /// Path to the `pcd` format point cloud.
    std::string path_;
};

/// \class PLYPointCloud
/// \brief Data class for `PLYPointCloud` contains the `fragment.ply`
/// point cloud mesh from the `Redwood Living Room` dataset.
class PLYPointCloud : public SingleDownloadDataset {
public:
    explicit PLYPointCloud(const std::string& data_root = "");

    /// \brief Returns path to the PLY format point cloud.
    [[nodiscard]] std::string GetPath() const { return path_; };

private:
    /// Path to the PLY format point cloud.
    std::string path_;
};

/// \class PTSPointCloud
/// \brief Data class for `PTSPointCloud` contains a sample point-cloud of PTS
/// format.
class PTSPointCloud : public SingleDownloadDataset {
public:
    explicit PTSPointCloud(const std::string& data_root = "");

    /// \brief Returns path to the PTS format point cloud.
    [[nodiscard]] std::string GetPath() const { return path_; };

private:
    /// Path to the PTS format point cloud.
    std::string path_;
};

/// \class SampleNYURGBDImage
/// \brief Data class for `SampleNYURGBDImage` contains a color image
/// `NYU_color.ppm` and a depth image `NYU_depth.pgm` sample from NYU RGBD
/// dataset.
class SampleNYURGBDImage : public SingleDownloadDataset {
public:
    explicit SampleNYURGBDImage(const std::string& data_root = "");

    /// \brief Returns path to color image sample.
    [[nodiscard]] std::string GetColorPath() const { return color_path_; };
    /// \brief Returns path to depth image sample.
    [[nodiscard]] std::string GetDepthPath() const { return depth_path_; };

private:
    /// Path to color image sample.
    std::string color_path_;
    /// Path to depth image sample.
    std::string depth_path_;
};

/// \class SampleSUNRGBDImage
/// \brief Data class for `SampleSUNRGBDImage` contains a color image
/// `SUN_color.jpg` and a depth image `SUN_depth.png` sample from SUN RGBD
/// dataset.
class SampleSUNRGBDImage : public SingleDownloadDataset {
public:
    explicit SampleSUNRGBDImage(const std::string& data_root = "");

    /// \brief Returns path to color image sample.
    [[nodiscard]] std::string GetColorPath() const { return color_path_; };
    /// \brief Returns path to depth image sample.
    [[nodiscard]] std::string GetDepthPath() const { return depth_path_; };

private:
    /// Path to color image sample.
    std::string color_path_;
    /// Path to depth image sample.
    std::string depth_path_;
};

/// \class SampleTUMRGBDImage
/// \brief Data class for `SampleTUMRGBDImage` contains a color image
/// `TUM_color.png` and a depth image `TUM_depth.png` sample from TUM RGBD
/// dataset.
class SampleTUMRGBDImage : public SingleDownloadDataset {
public:
    explicit SampleTUMRGBDImage(const std::string& data_root = "");

    /// \brief Returns path to color image sample.
    [[nodiscard]] std::string GetColorPath() const { return color_path_; };
    /// \brief Returns path to depth image sample.
    [[nodiscard]] std::string GetDepthPath() const { return depth_path_; };

private:
    /// Path to color image sample.
    std::string color_path_;
    /// Path to depth image sample.
    std::string depth_path_;
};

/// \class SampleRedwoodRGBDImages
/// \brief Data class for `SampleRedwoodRGBDImages` contains a sample set of 5
/// color and depth images from Redwood RGBD dataset living-room1.
// Additionally, it also contains camera trajectory log, camera odometry log,
// rgbd match, and point cloud reconstruction obtained using TSDF.
class SampleRedwoodRGBDImages : public SingleDownloadDataset {
public:
    explicit SampleRedwoodRGBDImages(const std::string& data_root = "");

    /// \brief Returns List of paths to color image samples of size 5.
    [[nodiscard]] std::vector<std::string> GetColorPaths() const { return color_paths_; };
    /// \brief Returns List of paths to depth image samples of size 5.
    [[nodiscard]] std::vector<std::string> GetDepthPaths() const { return depth_paths_; };

    /// \brief Returns path to camera trajectory log file `trajectory.log`.
    [[nodiscard]] std::string GetTrajectoryLogPath() const { return trajectory_log_path_; };
    /// \brief Returns path to camera trajectory log file `odometry.log`.
    [[nodiscard]] std::string GetOdometryLogPath() const { return odometry_log_path_; };
    /// \brief Returns path to color and depth image match file `rgbd.match`.
    [[nodiscard]] std::string GetRGBDMatchPath() const { return rgbd_match_path_; };
    /// \brief Returns path to pointcloud reconstruction from TSDF.
    [[nodiscard]] std::string GetReconstructionPath() const { return reconstruction_path_; };
    /// \brief Returns path to pinhole camera intrinsic (json).
    [[nodiscard]] std::string GetCameraIntrinsicPath() const { return camera_intrinsic_path_; }

private:
    /// List of paths to color image samples of size 5.
    std::vector<std::string> color_paths_;
    /// List of paths to depth image samples of size 5.
    std::vector<std::string> depth_paths_;

    /// Path to camera trajectory log file `trajectory.log`.
    std::string trajectory_log_path_;
    /// Path to camera odometry log file `odometry.log`.
    std::string odometry_log_path_;
    /// Path to color and depth image match file `rgbd.match`.
    std::string rgbd_match_path_;
    /// Path to pointcloud reconstruction from TSDF.
    std::string reconstruction_path_;
    /// Path to pinhole camera intrinsic.
    std::string camera_intrinsic_path_;
};

/// \class SampleFountainRGBDImages
/// \brief Data class for `SampleFountainRGBDImages` contains a sample set of
/// 33 color and depth images from the `Fountain RGBD dataset`.
class SampleFountainRGBDImages : public SingleDownloadDataset {
public:
    explicit SampleFountainRGBDImages(const std::string& data_root = "");

    /// \brief Returns List of paths to color image samples of size 33.
    [[nodiscard]] std::vector<std::string> GetColorPaths() const { return color_paths_; };
    /// \brief Returns List of paths to depth image samples of size 5.
    [[nodiscard]] std::vector<std::string> GetDepthPaths() const { return depth_paths_; };
    /// \brief Returns path to camera poses at keyfragmes log file `key.log`.
    [[nodiscard]] std::string GetKeyframePosesLogPath() const { return keyframe_poses_log_path_; };
    /// \brief Returns path to mesh reconstruction.
    [[nodiscard]] std::string GetReconstructionPath() const { return reconstruction_path_; };

private:
    std::vector<std::string> color_paths_;
    std::vector<std::string> depth_paths_;
    std::string keyframe_poses_log_path_;
    std::string reconstruction_path_;
};

/// \class SampleL515Bag
/// \brief Data class for `SampleL515Bag` contains the `SampleL515Bag.bag` file.
class SampleL515Bag : public SingleDownloadDataset {
public:
    explicit SampleL515Bag(const std::string& data_root = "");

    /// \brief Returns path to the `SampleL515Bag.bag` file.
    [[nodiscard]] std::string GetPath() const { return path_; };

private:
    /// Path to `SampleL515Bag.bag` file.
    std::string path_;
};

/// \class EaglePointCloud
/// \brief Data class for `EaglePointCloud` contains the `EaglePointCloud.ply`
/// file.
class EaglePointCloud : public SingleDownloadDataset {
public:
    explicit EaglePointCloud(const std::string& data_root = "");

    /// \brief Returns path to the `EaglePointCloud.ply` file.
    [[nodiscard]] std::string GetPath() const { return path_; };

private:
    /// Path to `EaglePointCloud.ply` file.
    std::string path_;
};

/// \class ArmadilloMesh
/// \brief Data class for `ArmadilloMesh` contains the `ArmadilloMesh.ply` from
/// the `Stanford 3D Scanning Repository`.
class ArmadilloMesh : public SingleDownloadDataset {
public:
    explicit ArmadilloMesh(const std::string& data_root = "");

    /// \brief Returns path to the `ArmadilloMesh.ply` file.
    [[nodiscard]] std::string GetPath() const { return path_; };

private:
    /// Path to the `ArmadilloMesh.ply` file.
    std::string path_;
};

/// \class BunnyMesh
/// \brief Data class for `BunnyMesh` contains the `BunnyMesh.ply` from the
/// `Stanford 3D Scanning Repository`.
class BunnyMesh : public SingleDownloadDataset {
public:
    explicit BunnyMesh(const std::string& data_root = "");

    /// \brief Returns path to the `BunnyMesh.ply` file.
    [[nodiscard]] std::string GetPath() const { return path_; };

private:
    /// Path to `BunnyMesh.ply` file.
    std::string path_;
};

/// \class KnotMesh
/// \brief Data class for `KnotMesh` contains the `KnotMesh.ply` file.
class KnotMesh : public SingleDownloadDataset {
public:
    explicit KnotMesh(const std::string& data_root = "");

    /// \brief Returns path to the `KnotMesh.ply` file.
    [[nodiscard]] std::string GetPath() const { return path_; };

private:
    /// Path to `KnotMesh.ply` file.
    std::string path_;
};

/// \class MonkeyModel
/// \brief Data class for `MonkeyModel` contains a monkey model file, along with
/// material and various other texture files. The model file can be accessed
/// using `GetPath()`, however in order to access the paths to the texture files
/// one may use `GetPath(filename)` method or get the unordered map of filename
/// to path using `GetPathMap()`.
class MonkeyModel : public SingleDownloadDataset {
public:
    explicit MonkeyModel(const std::string& data_root = "");

    /// \brief Returns path to the `filename`. By default it returns the path to
    /// `monkey.obj` file. Refer documentation page for available options.
    [[nodiscard]] std::string GetPath(const std::string& filename = "monkey_model") const {
        return map_filename_to_path_.at(filename);
    };

    /// \brief Returns the map of filename to path. Refer documentation page for
    /// available options.
    [[nodiscard]] std::unordered_map<std::string, std::string> GetPathMap() const { return map_filename_to_path_; }

private:
    /// Map to path for the available filenames.
    std::unordered_map<std::string, std::string> map_filename_to_path_;
};

/// \class SwordModel
/// \brief Data class for `SwordModel` contains a sword model file, along with
/// material and various other texture files. The model file can be accessed
/// using `GetPath()`, however in order to access the paths to the texture files
/// one may use `GetPath(filename)` method or get the unordered map of filename
/// to path using `GetPathMap()`.
class SwordModel : public SingleDownloadDataset {
public:
    explicit SwordModel(const std::string& data_root = "");

    /// \brief Returns path to the `filename`. By default it returns the path to
    /// `sword.obj` file. Refer documentation page for available options.
    [[nodiscard]] std::string GetPath(const std::string& filename = "sword_model") const {
        return map_filename_to_path_.at(filename);
    };

    /// \brief Returns the map of filename to path. Refer documentation page for
    /// available options.
    [[nodiscard]] std::unordered_map<std::string, std::string> GetPathMap() const { return map_filename_to_path_; }

private:
    /// Map to path for the available filenames.
    std::unordered_map<std::string, std::string> map_filename_to_path_;
};

/// \class CrateModel
/// \brief Data class for `CrateModel` contains a sword model file, along with
/// material and various other texture files. The model file can be accessed
/// using `GetPath()`, however in order to access the paths to the texture files
/// one may use `GetPath(filename)` method or get the unordered map of filename
/// to path using `GetPathMap()`.
class CrateModel : public SingleDownloadDataset {
public:
    explicit CrateModel(const std::string& data_root = "");

    /// \brief Returns path to the `filename`. By default it returns the path to
    /// `crate.obj` file. Refer documentation page for available options.
    [[nodiscard]] std::string GetPath(const std::string& filename = "crate_model") const {
        return map_filename_to_path_.at(filename);
    };

    /// \brief Returns the map of filename to path. Refer documentation page for
    /// available options.
    [[nodiscard]] std::unordered_map<std::string, std::string> GetPathMap() const { return map_filename_to_path_; }

private:
    /// Map to path for the available filenames.
    std::unordered_map<std::string, std::string> map_filename_to_path_;
};

/// \class FlightHelmetModel
/// \brief Data class for `FlightHelmetModel` contains a flight helmet model
/// file, along with material and various other texture files. The model file
/// can be accessed using `GetPath()`, however in order to access the paths to
/// the texture files one may use `GetPath(filename)` method or get the
/// unordered map of filename to path using `GetPathMap()`.
class FlightHelmetModel : public SingleDownloadDataset {
public:
    explicit FlightHelmetModel(const std::string& data_root = "");

    /// \brief Returns path to the `filename`. By default it returns the path to
    /// `FlightHelmet.gltf` file. Refer documentation page for available
    /// options.
    [[nodiscard]] std::string GetPath(const std::string& filename = "flight_helmet") const {
        return map_filename_to_path_.at(filename);
    };

    /// \brief Returns the map of filename to path. Refer documentation page for
    /// available options.
    [[nodiscard]] std::unordered_map<std::string, std::string> GetPathMap() const { return map_filename_to_path_; }

private:
    /// Map to path for the available filenames.
    std::unordered_map<std::string, std::string> map_filename_to_path_;
};

/// \class MetalTexture
/// \brief Data class for `MetalTexture` contains albedo, normal, roughness and
/// metallic texture files for metal based material.
class MetalTexture : public SingleDownloadDataset {
public:
    explicit MetalTexture(const std::string& data_root = "");
    /// \brief Returns the path to albedo color texture image.
    [[nodiscard]] std::string GetAlbedoTexturePath() const { return map_filename_to_path_.at("albedo"); };
    /// \brief Returns the path to normal texture image.
    [[nodiscard]] std::string GetNormalTexturePath() const { return map_filename_to_path_.at("normal"); };
    /// \brief Returns the path to roughness texture image.
    [[nodiscard]] std::string GetRoughnessTexturePath() const { return map_filename_to_path_.at("roughness"); };
    /// \brief Returns the path to metallic texture image.
    [[nodiscard]] std::string GetMetallicTexturePath() const { return map_filename_to_path_.at("metallic"); };
    /// \brief Returns the map of filename to path. Refer documentation page for
    /// available options.
    [[nodiscard]] std::unordered_map<std::string, std::string> GetPathMap() const { return map_filename_to_path_; }

private:
    /// Map to path for the available filenames.
    std::unordered_map<std::string, std::string> map_filename_to_path_;
};

/// \class PaintedPlasterTexture
/// \brief Data class for `PaintedPlasterTexture` contains albedo, normal and
/// roughness texture files for painted plaster based material.
class PaintedPlasterTexture : public SingleDownloadDataset {
public:
    explicit PaintedPlasterTexture(const std::string& data_root = "");
    /// \brief Returns the path to albedo color texture image.
    [[nodiscard]] std::string GetAlbedoTexturePath() const { return map_filename_to_path_.at("albedo"); };
    /// \brief Returns the path to normal texture image.
    [[nodiscard]] std::string GetNormalTexturePath() const { return map_filename_to_path_.at("normal"); };
    /// \brief Returns the path to roughness texture image.
    [[nodiscard]] std::string GetRoughnessTexturePath() const { return map_filename_to_path_.at("roughness"); };
    /// \brief Returns the map of filename to path. Refer documentation page for
    /// available options.
    [[nodiscard]] std::unordered_map<std::string, std::string> GetPathMap() const { return map_filename_to_path_; }

private:
    /// Map to path for the available filenames.
    std::unordered_map<std::string, std::string> map_filename_to_path_;
};

/// \class TilesTexture
/// \brief Data class for `TilesTexture` contains albedo, normal and roughness
/// texture files for tiles based material.
class TilesTexture : public SingleDownloadDataset {
public:
    explicit TilesTexture(const std::string& data_root = "");
    /// \brief Returns the path to albedo color texture image.
    [[nodiscard]] std::string GetAlbedoTexturePath() const { return map_filename_to_path_.at("albedo"); };
    /// \brief Returns the path to normal texture image.
    [[nodiscard]] std::string GetNormalTexturePath() const { return map_filename_to_path_.at("normal"); };
    /// \brief Returns the path to roughness texture image.
    [[nodiscard]] std::string GetRoughnessTexturePath() const { return map_filename_to_path_.at("roughness"); };
    /// \brief Returns the map of filename to path. Refer documentation page for
    /// available options.
    [[nodiscard]] std::unordered_map<std::string, std::string> GetPathMap() const { return map_filename_to_path_; }

private:
    /// Map to path for the available filenames.
    std::unordered_map<std::string, std::string> map_filename_to_path_;
};

/// \class TerrazzoTexture
/// \brief Data class for `TerrazzoTexture` contains albedo, normal and
/// roughness texture files for terrazzo based material.
class TerrazzoTexture : public SingleDownloadDataset {
public:
    explicit TerrazzoTexture(const std::string& data_root = "");
    /// \brief Returns the path to albedo color texture image.
    [[nodiscard]] std::string GetAlbedoTexturePath() const { return map_filename_to_path_.at("albedo"); };
    /// \brief Returns the path to normal texture image.
    [[nodiscard]] std::string GetNormalTexturePath() const { return map_filename_to_path_.at("normal"); };
    /// \brief Returns the path to roughness texture image.
    [[nodiscard]] std::string GetRoughnessTexturePath() const { return map_filename_to_path_.at("roughness"); };
    /// \brief Returns the map of filename to path. Refer documentation page for
    /// available options.
    [[nodiscard]] std::unordered_map<std::string, std::string> GetPathMap() const { return map_filename_to_path_; }

private:
    /// Map to path for the available filenames.
    std::unordered_map<std::string, std::string> map_filename_to_path_;
};

/// \class WoodTexture
/// \brief Data class for `WoodTexture` contains albedo, normal and roughness
/// texture files for wood based material.
class WoodTexture : public SingleDownloadDataset {
public:
    explicit WoodTexture(const std::string& data_root = "");
    /// \brief Returns the path to albedo color texture image.
    [[nodiscard]] std::string GetAlbedoTexturePath() const { return map_filename_to_path_.at("albedo"); };
    /// \brief Returns the path to normal texture image.
    [[nodiscard]] std::string GetNormalTexturePath() const { return map_filename_to_path_.at("normal"); };
    /// \brief Returns the path to roughness texture image.
    [[nodiscard]] std::string GetRoughnessTexturePath() const { return map_filename_to_path_.at("roughness"); };
    /// \brief Returns the map of filename to path. Refer documentation page for
    /// available options.
    [[nodiscard]] std::unordered_map<std::string, std::string> GetPathMap() const { return map_filename_to_path_; }

private:
    /// Map to path for the available filenames.
    std::unordered_map<std::string, std::string> map_filename_to_path_;
};

/// \class WoodFloorTexture
/// \brief Data class for `WoodFloorTexture` contains albedo, normal and
/// roughness texture files for wooden floor based material.
class WoodFloorTexture : public SingleDownloadDataset {
public:
    explicit WoodFloorTexture(const std::string& data_root = "");
    /// \brief Returns the path to albedo color texture image.
    [[nodiscard]] std::string GetAlbedoTexturePath() const { return map_filename_to_path_.at("albedo"); };
    /// \brief Returns the path to normal texture image.
    [[nodiscard]] std::string GetNormalTexturePath() const { return map_filename_to_path_.at("normal"); };
    /// \brief Returns the path to roughness texture image.
    [[nodiscard]] std::string GetRoughnessTexturePath() const { return map_filename_to_path_.at("roughness"); };
    /// \brief Returns the map of filename to path. Refer documentation page for
    /// available options.
    [[nodiscard]] std::unordered_map<std::string, std::string> GetPathMap() const { return map_filename_to_path_; }

private:
    /// Map to path for the available filenames.
    std::unordered_map<std::string, std::string> map_filename_to_path_;
};

/// \class JuneauImage
/// \brief Data class for `JuneauImage` contains the `JuneauImage.jpg` file.
class JuneauImage : public SingleDownloadDataset {
public:
    explicit JuneauImage(const std::string& data_root = "");

    /// \brief Returns path to the `JuneauImage.jgp` file.
    [[nodiscard]] std::string GetPath() const { return path_; };

private:
    /// Path to `JuneauImage.jgp` file.
    std::string path_;
};

/// \class LivingRoomPointClouds
/// \brief Dataset class for `LivingRoomPointClouds` contains 57 point
/// clouds of binary PLY format.
/// \copyright Creative Commons 3.0 (CC BY 3.0).
class LivingRoomPointClouds : public SingleDownloadDataset {
public:
    explicit LivingRoomPointClouds(const std::string& data_root = "");

    /// \brief Returns list of paths to ply point-cloud fragments of size 57.
    [[nodiscard]] std::vector<std::string> GetPaths() const { return paths_; }
    /// \brief Returns path to the ply point-cloud fragment at index (from 0 to
    /// 56). Use `GetPaths(0)`, `GetPaths(1)` ... `GetPaths(56)` to access the
    /// paths.
    [[nodiscard]] std::string GetPaths(size_t index) const;

private:
    /// List of paths to ply point-cloud fragments of size 57.
    std::vector<std::string> paths_;
};

/// \class OfficePointClouds
/// \brief Dataset class for `OfficePointClouds` contains 53 point clouds
/// of binary PLY format.
/// \copyright Creative Commons 3.0 (CC BY 3.0).
class OfficePointClouds : public SingleDownloadDataset {
public:
    explicit OfficePointClouds(const std::string& data_root = "");

    /// \brief Returns list of paths to ply point-cloud fragments of size 52.
    [[nodiscard]] std::vector<std::string> GetPaths() const { return paths_; }
    /// \brief Returns path to the ply point-cloud fragment at index (from 0 to
    /// 52). Use `GetPaths(0)`, `GetPaths(1)` ... `GetPaths(52)` to access the
    /// paths.
    [[nodiscard]] std::string GetPaths(size_t index) const;

private:
    /// List of paths to ply point-cloud fragments of size 53.
    std::vector<std::string> paths_;
};

}  // namespace vox::data
