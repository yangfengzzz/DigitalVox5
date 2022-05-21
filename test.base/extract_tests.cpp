//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "dataset.h"
#include "download.h"
#include "extract.h"
#include "file_system.h"
#include "tests.h"

namespace vox::tests {

TEST(Extract, ExtractFromZIP) {
    // Directory relative to `data_root`, where files will be temp. downloaded
    // for this test.
    const std::string prefix = "test_extract";
    const std::string extract_dir = data::LocateDataRoot() + "/" + prefix;
    EXPECT_TRUE(utility::filesystem::DeleteDirectory(extract_dir));

    // Download the `test_data_v2_00.zip` test data.
    std::string url =
            "https://github.com/isl-org/open3d_downloads/releases/download/"
            "data-manager/test_data_v2_00.zip";
    std::string md5 = "bc47a5e33d33e717259e3a37fa5eebef";
    std::string file_path = extract_dir + "/test_data_v2_00.zip";
    // This download shall work.
    EXPECT_EQ(utility::DownloadFromURL(url, md5, prefix), file_path);

    // Extract the test zip file.
    EXPECT_NO_THROW(utility::Extract(file_path, extract_dir));
    url = "https://github.com/isl-org/open3d_downloads/releases/download/"
          "data-manager/test_data_v2_00.tar.xz";
    md5 = "7c682c7af4ef9bda1fc854b008ae2bef";
    file_path = extract_dir + "/test_data_v2_00.tar.xz";
    EXPECT_EQ(utility::DownloadFromURL(url, md5, prefix), file_path);

    // Currently only `.zip` files are supported.
    EXPECT_ANY_THROW(utility::Extract(file_path, extract_dir));

    // Clean up.
    EXPECT_TRUE(utility::filesystem::DeleteDirectory(extract_dir));
}

}  // namespace vox::tests
