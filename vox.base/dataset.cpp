//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.base/dataset.h"

#include <string>
#include <utility>

#include "vox.base/download.h"
#include "vox.base/extract.h"
#include "vox.base/file_system.h"
#include "vox.base/logging.h"

namespace vox::data {

std::string LocateDataRoot() {
    std::string data_root;
    if (const char* env_p = std::getenv("OPEN3D_DATA_ROOT")) {
        data_root = std::string(env_p);
    }
    if (data_root.empty()) {
        data_root = utility::filesystem::GetHomeDirectory() + "/open3d_data";
    }
    return data_root;
}

Dataset::Dataset(std::string prefix, const std::string& data_root) : prefix_(std::move(prefix)) {
    if (data_root.empty()) {
        data_root_ = LocateDataRoot();
    } else {
        data_root_ = data_root;
    }
    if (prefix_.empty()) {
        LOGE("prefix cannot be empty.")
    }
}

SingleDownloadDataset::SingleDownloadDataset(const std::string& prefix,
                                             const std::vector<std::string>& urls,
                                             const std::string& md5,
                                             const bool no_extract,
                                             const std::string& data_root)
    : Dataset(prefix, data_root) {
    const std::string kFilename = utility::filesystem::GetFileNameWithoutDirectory(urls[0]);

    const bool kIsExtractPresent = utility::filesystem::DirectoryExists(Dataset::GetExtractDir());

    if (!kIsExtractPresent) {
        // `download_dir` is relative path from `${data_root}`.
        const std::string kDownloadDir = "download/" + GetPrefix();
        const std::string kDownloadFilePath = utility::DownloadFromURL(urls, md5, kDownloadDir, data_root_);

        // Extract / Copy data.
        if (!no_extract) {
            utility::Extract(kDownloadFilePath, Dataset::GetExtractDir());
        } else {
            utility::filesystem::MakeDirectoryHierarchy(Dataset::GetExtractDir());
            utility::filesystem::Copy(kDownloadFilePath, Dataset::GetExtractDir());
        }
    }
}

DemoICPPointClouds::DemoICPPointClouds(const std::string& data_root)
    : SingleDownloadDataset("DemoICPPointClouds",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220301-data/DemoICPPointClouds.zip"},
                            "596cffe5f9c587045e7397ad70754de9",
                            /*no_extract =*/false,
                            data_root) {
    for (int i = 0; i < 3; ++i) {
        paths_.push_back(Dataset::GetExtractDir() + "/cloud_bin_" + std::to_string(i) + ".pcd");
    }
    transformation_log_path_ = Dataset::GetExtractDir() + "/init.log";
}

std::string DemoICPPointClouds::GetPaths(size_t index) const {
    if (index > 2) {
        LOGE("Invalid index. Expected index between 0 to 2 but got {}.", index)
    }
    return paths_[index];
}

DemoColoredICPPointClouds::DemoColoredICPPointClouds(const std::string& data_root)
    : SingleDownloadDataset("DemoColoredICPPointClouds",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220201-data/DemoColoredICPPointClouds.zip"},
                            "bf8d469e892d76f2e69e1213207c0e30",
                            /*no_extract =*/false,
                            data_root) {
    paths_.push_back(Dataset::GetExtractDir() + "/frag_115.ply");
    paths_.push_back(Dataset::GetExtractDir() + "/frag_116.ply");
}

std::string DemoColoredICPPointClouds::GetPaths(size_t index) const {
    if (index > 1) {
        LOGE("Invalid index. Expected index between 0 to 1 but got {}.", index)
    }
    return paths_[index];
}

DemoCropPointCloud::DemoCropPointCloud(const std::string& data_root)
    : SingleDownloadDataset("DemoCropPointCloud",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220201-data/DemoCropPointCloud.zip"},
                            "12dbcdddd3f0865d8312929506135e23",
                            /*no_extract =*/false,
                            data_root) {
    const std::string kExtractDir = Dataset::GetExtractDir();
    point_cloud_path_ = kExtractDir + "/fragment.ply";
    cropped_json_path_ = kExtractDir + "/cropped.json";
}

DemoFeatureMatchingPointClouds::DemoFeatureMatchingPointClouds(const std::string& data_root)
    : SingleDownloadDataset("DemoFeatureMatchingPointClouds",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220201-data/DemoFeatureMatchingPointClouds.zip"},
                            "02f0703ce0cbf4df78ce2602ae33fc79",
                            /*no_extract =*/false,
                            data_root) {
    const std::string kExtractDir = Dataset::GetExtractDir();
    point_cloud_paths_ = {kExtractDir + "/cloud_bin_0.pcd", kExtractDir + "/cloud_bin_1.pcd"};
    fpfh_feature_paths_ = {kExtractDir + "/cloud_bin_0.fpfh.bin", kExtractDir + "/cloud_bin_1.fpfh.bin"};
    l32d_feature_paths_ = {kExtractDir + "/cloud_bin_0.d32.bin", kExtractDir + "/cloud_bin_1.d32.bin"};
}

DemoPoseGraphOptimization::DemoPoseGraphOptimization(const std::string& data_root)
    : SingleDownloadDataset("DemoPoseGraphOptimization",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220201-data/DemoPoseGraphOptimization.zip"},
                            "af085b28d79dea7f0a50aef50c96b62c",
                            /*no_extract =*/false,
                            data_root) {
    const std::string kExtractDir = Dataset::GetExtractDir();
    pose_graph_fragment_path_ = kExtractDir + "/pose_graph_example_fragment.json";
    pose_graph_global_path_ = kExtractDir + "/pose_graph_example_global.json";
}

DemoCustomVisualization::DemoCustomVisualization(const std::string& data_root)
    : SingleDownloadDataset("DemoCustomVisualization",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220301-data/DemoCustomVisualization.zip"},
                            "04cb716145c51d0119b59c7876249891",
                            /*no_extract =*/false,
                            data_root) {
    const std::string kExtractDir = Dataset::GetExtractDir();
    point_cloud_path_ = kExtractDir + "/fragment.ply";
    camera_trajectory_path_ = kExtractDir + "/camera_trajectory.json";
    render_option_path_ = kExtractDir + "/renderoption.json";
}

PCDPointCloud::PCDPointCloud(const std::string& data_root)
    : SingleDownloadDataset("PCDPointCloud",
                            {"https://github.com/isl-org/open3d_downloads/releases/"
                             "download/20220201-data/fragment.pcd"},
                            "f3a613fd2bdecd699aabdd858fb29606",
                            /*no_extract =*/true,
                            data_root) {
    path_ = Dataset::GetExtractDir() + "/fragment.pcd";
}

PLYPointCloud::PLYPointCloud(const std::string& data_root)
    : SingleDownloadDataset("PLYPointCloud",
                            {"https://github.com/isl-org/open3d_downloads/releases/"
                             "download/20220201-data/fragment.ply"},
                            "831ecffd4d7cbbbe02494c5c351aa6e5",
                            /*no_extract =*/true,
                            data_root) {
    path_ = Dataset::GetExtractDir() + "/fragment.ply";
}

PTSPointCloud::PTSPointCloud(const std::string& data_root)
    : SingleDownloadDataset("PTSPointCloud",
                            {"https://github.com/isl-org/open3d_downloads/releases/"
                             "download/20220301-data/point_cloud_sample1.pts"},
                            "5c2c618b703d0161e6e333fcbf55a1e9",
                            /*no_extract =*/true,
                            data_root) {
    path_ = Dataset::GetExtractDir() + "/point_cloud_sample1.pts";
}

SampleNYURGBDImage::SampleNYURGBDImage(const std::string& data_root)
    : SingleDownloadDataset("SampleNYURGBDImage",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220201-data/SampleNYURGBDImage.zip"},
                            "b0baaf892c7ff9b202eb5fb40c5f7b58",
                            /*no_extract =*/false,
                            data_root) {
    color_path_ = Dataset::GetExtractDir() + "/NYU_color.ppm";
    depth_path_ = Dataset::GetExtractDir() + "/NYU_depth.pgm";
}

SampleSUNRGBDImage::SampleSUNRGBDImage(const std::string& data_root)
    : SingleDownloadDataset("SampleSUNRGBDImage",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220201-data/SampleSUNRGBDImage.zip"},
                            "b1a430586547c8986bdf8b36179a8e67",
                            /*no_extract =*/false,
                            data_root) {
    color_path_ = Dataset::GetExtractDir() + "/SUN_color.jpg";
    depth_path_ = Dataset::GetExtractDir() + "/SUN_depth.png";
}

SampleTUMRGBDImage::SampleTUMRGBDImage(const std::string& data_root)
    : SingleDownloadDataset("SampleTUMRGBDImage",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220201-data/SampleTUMRGBDImage.zip"},
                            "91758d42b142dbad7b0d90e857ad47a8",
                            /*no_extract =*/false,
                            data_root) {
    color_path_ = Dataset::GetExtractDir() + "/TUM_color.png";
    depth_path_ = Dataset::GetExtractDir() + "/TUM_depth.png";
}

SampleRedwoodRGBDImages::SampleRedwoodRGBDImages(const std::string& data_root)
    : SingleDownloadDataset("SampleRedwoodRGBDImages",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220301-data/SampleRedwoodRGBDImages.zip"},
                            "43971c5f690c9cfc52dda8c96a0140ee",
                            /*no_extract =*/false,
                            data_root) {
    const std::string kExtractDir = Dataset::GetExtractDir();

    color_paths_ = {kExtractDir + "/color/00000.jpg", kExtractDir + "/color/00001.jpg",
                    kExtractDir + "/color/00002.jpg", kExtractDir + "/color/00003.jpg",
                    kExtractDir + "/color/00004.jpg"};

    depth_paths_ = {kExtractDir + "/depth/00000.png", kExtractDir + "/depth/00001.png",
                    kExtractDir + "/depth/00002.png", kExtractDir + "/depth/00003.png",
                    kExtractDir + "/depth/00004.png"};

    trajectory_log_path_ = kExtractDir + "/trajectory.log";
    odometry_log_path_ = kExtractDir + "/odometry.log";
    rgbd_match_path_ = kExtractDir + "/rgbd.match";
    reconstruction_path_ = kExtractDir + "/example_tsdf_pcd.ply";
    camera_intrinsic_path_ = kExtractDir + "/camera_primesense.json";
}

SampleFountainRGBDImages::SampleFountainRGBDImages(const std::string& data_root)
    : SingleDownloadDataset("SampleFountainRGBDImages",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220201-data/SampleFountainRGBDImages.zip"},
                            "c6c1b2171099f571e2a78d78675df350",
                            /*no_extract =*/false,
                            data_root) {
    const std::string kExtractDir = Dataset::GetExtractDir();
    color_paths_ = {kExtractDir + "/image/0000010-000001228920.jpg", kExtractDir + "/image/0000031-000004096400.jpg",
                    kExtractDir + "/image/0000044-000005871507.jpg", kExtractDir + "/image/0000064-000008602440.jpg",
                    kExtractDir + "/image/0000110-000014883587.jpg", kExtractDir + "/image/0000156-000021164733.jpg",
                    kExtractDir + "/image/0000200-000027172787.jpg", kExtractDir + "/image/0000215-000029220987.jpg",
                    kExtractDir + "/image/0000255-000034682853.jpg", kExtractDir + "/image/0000299-000040690907.jpg",
                    kExtractDir + "/image/0000331-000045060400.jpg", kExtractDir + "/image/0000368-000050112627.jpg",
                    kExtractDir + "/image/0000412-000056120680.jpg", kExtractDir + "/image/0000429-000058441973.jpg",
                    kExtractDir + "/image/0000474-000064586573.jpg", kExtractDir + "/image/0000487-000066361680.jpg",
                    kExtractDir + "/image/0000526-000071687000.jpg", kExtractDir + "/image/0000549-000074827573.jpg",
                    kExtractDir + "/image/0000582-000079333613.jpg", kExtractDir + "/image/0000630-000085887853.jpg",
                    kExtractDir + "/image/0000655-000089301520.jpg", kExtractDir + "/image/0000703-000095855760.jpg",
                    kExtractDir + "/image/0000722-000098450147.jpg", kExtractDir + "/image/0000771-000105140933.jpg",
                    kExtractDir + "/image/0000792-000108008413.jpg", kExtractDir + "/image/0000818-000111558627.jpg",
                    kExtractDir + "/image/0000849-000115791573.jpg", kExtractDir + "/image/0000883-000120434160.jpg",
                    kExtractDir + "/image/0000896-000122209267.jpg", kExtractDir + "/image/0000935-000127534587.jpg",
                    kExtractDir + "/image/0000985-000134361920.jpg", kExtractDir + "/image/0001028-000140233427.jpg",
                    kExtractDir + "/image/0001061-000144739467.jpg"};

    depth_paths_ = {kExtractDir + "/depth/0000038-000001234662.png", kExtractDir + "/depth/0000124-000004104418.png",
                    kExtractDir + "/depth/0000177-000005872988.png", kExtractDir + "/depth/0000259-000008609267.png",
                    kExtractDir + "/depth/0000447-000014882686.png", kExtractDir + "/depth/0000635-000021156105.png",
                    kExtractDir + "/depth/0000815-000027162570.png", kExtractDir + "/depth/0000877-000029231463.png",
                    kExtractDir + "/depth/0001040-000034670651.png", kExtractDir + "/depth/0001220-000040677116.png",
                    kExtractDir + "/depth/0001351-000045048488.png", kExtractDir + "/depth/0001503-000050120614.png",
                    kExtractDir + "/depth/0001683-000056127079.png", kExtractDir + "/depth/0001752-000058429557.png",
                    kExtractDir + "/depth/0001937-000064602868.png", kExtractDir + "/depth/0001990-000066371438.png",
                    kExtractDir + "/depth/0002149-000071677149.png", kExtractDir + "/depth/0002243-000074813859.png",
                    kExtractDir + "/depth/0002378-000079318707.png", kExtractDir + "/depth/0002575-000085892450.png",
                    kExtractDir + "/depth/0002677-000089296113.png", kExtractDir + "/depth/0002874-000095869855.png",
                    kExtractDir + "/depth/0002951-000098439288.png", kExtractDir + "/depth/0003152-000105146507.png",
                    kExtractDir + "/depth/0003238-000108016262.png", kExtractDir + "/depth/0003344-000111553403.png",
                    kExtractDir + "/depth/0003471-000115791298.png", kExtractDir + "/depth/0003610-000120429623.png",
                    kExtractDir + "/depth/0003663-000122198194.png", kExtractDir + "/depth/0003823-000127537274.png",
                    kExtractDir + "/depth/0004028-000134377970.png", kExtractDir + "/depth/0004203-000140217589.png",
                    kExtractDir + "/depth/0004339-000144755807.png"};

    keyframe_poses_log_path_ = kExtractDir + "/scene/key.log";
    reconstruction_path_ = kExtractDir + "/scene/integrated.ply";
}

SampleL515Bag::SampleL515Bag(const std::string& data_root)
    : SingleDownloadDataset("SampleL515Bag",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220301-data/SampleL515Bag.zip"},
                            "9770eeb194c78103037dbdbec78b9c8c",
                            /*no_extract =*/false,
                            data_root) {
    path_ = Dataset::GetExtractDir() + "/L515_test_s.bag";
}

EaglePointCloud::EaglePointCloud(const std::string& data_root)
    : SingleDownloadDataset("EaglePointCloud",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220201-data/EaglePointCloud.ply"},
                            "e4e6c77bc548e7eb7548542a0220ad78",
                            /*no_extract =*/true,
                            data_root) {
    path_ = Dataset::GetExtractDir() + "/EaglePointCloud.ply";
}

ArmadilloMesh::ArmadilloMesh(const std::string& data_root)
    : SingleDownloadDataset("ArmadilloMesh",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220201-data/ArmadilloMesh.ply"},
                            "9e68ff1b1cc914ed88cd84f6a8235021",
                            /*no_extract =*/true,
                            data_root) {
    path_ = Dataset::GetExtractDir() + "/ArmadilloMesh.ply";
}

BunnyMesh::BunnyMesh(const std::string& data_root)
    : SingleDownloadDataset("BunnyMesh",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220201-data/BunnyMesh.ply"},
                            "568f871d1a221ba6627569f1e6f9a3f2",
                            /*no_extract =*/true,
                            data_root) {
    path_ = Dataset::GetExtractDir() + "/BunnyMesh.ply";
}

KnotMesh::KnotMesh(const std::string& data_root)
    : SingleDownloadDataset("KnotMesh",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220201-data/KnotMesh.ply"},
                            "bfc9f132ecdfb7f9fdc42abf620170fc",
                            /*no_extract =*/true,
                            data_root) {
    path_ = Dataset::GetExtractDir() + "/KnotMesh.ply";
}

MonkeyModel::MonkeyModel(const std::string& data_root)
    : SingleDownloadDataset("MonkeyModel",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220301-data/MonkeyModel.zip"},
                            "fc330bf4fd8e022c1e5ded76139785d4",
                            /*no_extract =*/false,
                            data_root) {
    const std::string kExtractDir = Dataset::GetExtractDir();
    map_filename_to_path_ = {{"albedo", kExtractDir + "/albedo.png"},
                             {"ao", kExtractDir + "/ao.png"},
                             {"metallic", kExtractDir + "/metallic.png"},
                             {"monkey_material", kExtractDir + "/monkey.mtl"},
                             {"monkey_model", kExtractDir + "/monkey.obj"},
                             {"monkey_solid_material", kExtractDir + "/monkey_solid.mtl"},
                             {"monkey_solid_model", kExtractDir + "/monkey_solid.obj"},
                             {"normal", kExtractDir + "/normal.png"},
                             {"roughness", kExtractDir + "/roughness.png"}};
}

SwordModel::SwordModel(const std::string& data_root)
    : SingleDownloadDataset("SwordModel",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220301-data/SwordModel.zip"},
                            "eb7df358b5c31c839f03c4b3b4157c04",
                            /*no_extract =*/false,
                            data_root) {
    const std::string kExtractDir = Dataset::GetExtractDir();
    map_filename_to_path_ = {{"sword_material", kExtractDir + "/UV.mtl"},
                             {"sword_model", kExtractDir + "/UV.obj"},
                             {"base_color", kExtractDir + "/UV_blinn1SG_BaseColor.png"},
                             {"metallic", kExtractDir + "/UV_blinn1SG_Metallic.png"},
                             {"normal", kExtractDir + "/UV_blinn1SG_Normal.png"},
                             {"roughness", kExtractDir + "/UV_blinn1SG_Roughness.png"}};
}

CrateModel::CrateModel(const std::string& data_root)
    : SingleDownloadDataset("CrateModel",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220301-data/CrateModel.zip"},
                            "20413eada103969bb3ca5df9aebc2034",
                            /*no_extract =*/false,
                            data_root) {
    const std::string kExtractDir = Dataset::GetExtractDir();
    map_filename_to_path_ = {{"crate_material", kExtractDir + "/crate.mtl"},
                             {"crate_model", kExtractDir + "/crate.obj"},
                             {"texture_image", kExtractDir + "/crate.jpg"}};
}

FlightHelmetModel::FlightHelmetModel(const std::string& data_root)
    : SingleDownloadDataset("FlightHelmetModel",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220301-data/FlightHelmetModel.zip"},
                            "597c3aa8b46955fff1949a8baa768bb4",
                            /*no_extract =*/false,
                            data_root) {
    const std::string kExtractDir = Dataset::GetExtractDir();
    map_filename_to_path_ = {
            {"flight_helmet", kExtractDir + "/FlightHelmet.gltf"},
            {"flight_helmet_bin", kExtractDir + "/FlightHelmet.bin"},
            {"mat_glass_plastic_base", kExtractDir + "/FlightHelmet_Materials_GlassPlasticMat_BaseColor.png"},
            {"mat_glass_plastic_normal", kExtractDir + "/FlightHelmet_Materials_GlassPlasticMat_Normal.png"},
            {"mat_glass_plastic_occlusion_rough_metal", kExtractDir + "/FlightHelmet_Materials_GlassPlasticMat_"
                                                                      "OcclusionRoughMetal.png"},
            {"mat_leather_parts_base", kExtractDir + "/FlightHelmet_Materials_LeatherPartsMat_BaseColor.png"},
            {"mat_leather_parts_normal", kExtractDir + "/FlightHelmet_Materials_LeatherPartsMat_Normal.png"},
            {"mat_leather_parts_occlusion_rough_metal", kExtractDir + "/FlightHelmet_Materials_LeatherPartsMat_"
                                                                      "OcclusionRoughMetal.png"},
            {"mat_lenses_base", kExtractDir + "/FlightHelmet_Materials_LensesMat_BaseColor.png"},
            {"mat_lenses_normal", kExtractDir + "/FlightHelmet_Materials_LensesMat_Normal.png"},
            {"mat_lenses_occlusion_rough_metal", kExtractDir + "/FlightHelmet_Materials_LensesMat_"
                                                               "OcclusionRoughMetal.png"},
            {"mat_metal_parts_base", kExtractDir + "/FlightHelmet_Materials_MetalPartsMat_BaseColor.png"},
            {"mat_metal_parts_normal", kExtractDir + "/FlightHelmet_Materials_MetalPartsMat_Normal.png"},
            {"mat_metal_parts_occlusion_rough_metal", kExtractDir + "/FlightHelmet_Materials_MetalPartsMat_"
                                                                    "OcclusionRoughMetal.png"},
            {"mat_rubber_wood_base", kExtractDir + "/FlightHelmet_Materials_RubberWoodMat_BaseColor.png"},
            {"mat_rubber_wood_normal", kExtractDir + "/FlightHelmet_Materials_RubberWoodMat_Normal.png"},
            {"mat_rubber_wood_occlusion_rough_metal", kExtractDir + "/FlightHelmet_Materials_RubberWoodMat_"
                                                                    "OcclusionRoughMetal.png"}};
}

MetalTexture::MetalTexture(const std::string& data_root)
    : SingleDownloadDataset("MetalTexture",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220301-data/MetalTexture.zip"},
                            "2b6a17e41157138868a2cd2926eedcc7",
                            /*no_extract =*/false,
                            data_root) {
    const std::string kExtractDir = Dataset::GetExtractDir();
    map_filename_to_path_ = {{"albedo", kExtractDir + "/Metal008_Color.jpg"},
                             {"normal", kExtractDir + "/Metal008_NormalDX.jpg"},
                             {"roughness", kExtractDir + "/Metal008_Roughness.jpg"},
                             {"metallic", kExtractDir + "/Metal008_Metalness.jpg"}};
}

PaintedPlasterTexture::PaintedPlasterTexture(const std::string& data_root)
    : SingleDownloadDataset("PaintedPlasterTexture",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220301-data/PaintedPlasterTexture.zip"},
                            "344096b29b06f14aac58f9ad73851dc2",
                            /*no_extract =*/false,
                            data_root) {
    const std::string kExtractDir = Dataset::GetExtractDir();
    map_filename_to_path_ = {{"albedo", kExtractDir + "/PaintedPlaster017_Color.jpg"},
                             {"normal", kExtractDir + "/PaintedPlaster017_NormalDX.jpg"},
                             {"roughness", kExtractDir + "/noiseTexture.png"}};
}

TilesTexture::TilesTexture(const std::string& data_root)
    : SingleDownloadDataset("TilesTexture",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220301-data/TilesTexture.zip"},
                            "23f47f1e8e1799216724eb0c837c274d",
                            /*no_extract =*/false,
                            data_root) {
    const std::string kExtractDir = Dataset::GetExtractDir();
    map_filename_to_path_ = {{"albedo", kExtractDir + "/Tiles074_Color.jpg"},
                             {"normal", kExtractDir + "/Tiles074_NormalDX.jpg"},
                             {"roughness", kExtractDir + "/Tiles074_Roughness.jpg"}};
}

TerrazzoTexture::TerrazzoTexture(const std::string& data_root)
    : SingleDownloadDataset("TerrazzoTexture",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220301-data/TerrazzoTexture.zip"},
                            "8d67f191fb5d80a27d8110902cac008e",
                            /*no_extract =*/false,
                            data_root) {
    const std::string kExtractDir = Dataset::GetExtractDir();
    map_filename_to_path_ = {{"albedo", kExtractDir + "/Terrazzo018_Color.jpg"},
                             {"normal", kExtractDir + "/Terrazzo018_NormalDX.jpg"},
                             {"roughness", kExtractDir + "/Terrazzo018_Roughness.jpg"}};
}

WoodTexture::WoodTexture(const std::string& data_root)
    : SingleDownloadDataset("WoodTexture",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220301-data/WoodTexture.zip"},
                            "28788c7ecc42d78d4d623afbab2301e9",
                            /*no_extract =*/false,
                            data_root) {
    const std::string kExtractDir = Dataset::GetExtractDir();
    map_filename_to_path_ = {{"albedo", kExtractDir + "/Wood049_Color.jpg"},
                             {"normal", kExtractDir + "/Wood049_NormalDX.jpg"},
                             {"roughness", kExtractDir + "/Wood049_Roughness.jpg"}};
}

WoodFloorTexture::WoodFloorTexture(const std::string& data_root)
    : SingleDownloadDataset("WoodFloorTexture",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220301-data/WoodFloorTexture.zip"},
                            "f11b3e50208095e87340049b9ac3c319",
                            /*no_extract =*/false,
                            data_root) {
    const std::string kExtractDir = Dataset::GetExtractDir();
    map_filename_to_path_ = {{"albedo", kExtractDir + "/WoodFloor050_Color.jpg"},
                             {"normal", kExtractDir + "/WoodFloor050_NormalDX.jpg"},
                             {"roughness", kExtractDir + "/WoodFloor050_Roughness.jpg"}};
}

JuneauImage::JuneauImage(const std::string& data_root)
    : SingleDownloadDataset("JuneauImage",
                            {"https://github.com/isl-org/open3d_downloads/releases/download/"
                             "20220201-data/JuneauImage.jpg"},
                            "a090f6342893bdf0caefd83c6debbecd",
                            /*no_extract =*/true,
                            data_root) {
    path_ = Dataset::GetExtractDir() + "/JuneauImage.jpg";
}

LivingRoomPointClouds::LivingRoomPointClouds(const std::string& data_root)
    : SingleDownloadDataset("LivingRoomPointClouds",
                            {"http://redwood-data.org/indoor/data/"
                             "livingroom1-fragments-ply.zip",
                             "https://github.com/isl-org/open3d_downloads/releases/"
                             "download/redwood/livingroom1-fragments-ply.zip"},
                            "36e0eb23a66ccad6af52c05f8390d33e",
                            /*no_extract =*/false,
                            data_root) {
    paths_.reserve(57);
    for (int i = 0; i < 57; ++i) {
        paths_.push_back(Dataset::GetExtractDir() + "/cloud_bin_" + std::to_string(i) + ".ply");
    }
}

std::string LivingRoomPointClouds::GetPaths(size_t index) const {
    if (index > 56) {
        LOGE("Invalid index. Expected index between 0 to 56 but got {}.", index)
    }
    return paths_[index];
}

OfficePointClouds::OfficePointClouds(const std::string& data_root)
    : SingleDownloadDataset("OfficePointClouds",
                            {"http://redwood-data.org/indoor/data/"
                             "office1-fragments-ply.zip",
                             "https://github.com/isl-org/open3d_downloads/releases/"
                             "download/redwood/office1-fragments-ply.zip"},
                            "c519fe0495b3c731ebe38ae3a227ac25",
                            /*no_extract =*/false,
                            data_root) {
    paths_.reserve(53);
    for (int i = 0; i < 53; ++i) {
        paths_.push_back(Dataset::GetExtractDir() + "/cloud_bin_" + std::to_string(i) + ".ply");
    }
}

std::string OfficePointClouds::GetPaths(size_t index) const {
    if (index > 52) {
        LOGE("Invalid index. Expected index between 0 to 52 but got {}.", index)
    }
    return paths_[index];
}

}  // namespace vox::data
