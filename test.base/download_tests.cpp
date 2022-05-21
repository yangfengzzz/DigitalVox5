//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "dataset.h"
#include "download.h"
#include "file_system.h"
#include "tests.h"

namespace vox::tests {

TEST(Downloader, DownloadAndVerify) {
    std::string url =
            "https://github.com/isl-org/open3d_downloads/releases/download/"
            "data-manager/test_data_00.zip";
    std::string md5 = "996987b27c4497dbb951ec056c9684f4";

    std::string prefix = "temp_test";
    std::string file_dir = data::LocateDataRoot() + "/" + prefix;
    std::string file_path = file_dir + "/" + "test_data_00.zip";
    EXPECT_TRUE(utility::filesystem::DeleteDirectory(file_dir));

    // This download shall work.
    EXPECT_EQ(utility::DownloadFromURL(url, md5, prefix), file_path);
    EXPECT_TRUE(utility::filesystem::DirectoryExists(file_dir));
    EXPECT_TRUE(utility::filesystem::FileExists(file_path));
    EXPECT_EQ(utility::GetMD5(file_path), md5);

    // This download shall be skipped as the file already exists (look at the
    // message).
    EXPECT_EQ(utility::DownloadFromURL(url, md5, prefix), file_path);

    // Mismatch md5.
    EXPECT_ANY_THROW(utility::DownloadFromURL(url, "00000000000000000000000000000000", prefix));

    // Clean up.
    EXPECT_TRUE(utility::filesystem::DeleteDirectory(file_dir));
}

}  // namespace vox::tests
