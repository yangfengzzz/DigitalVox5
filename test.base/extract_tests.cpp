// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018-2021 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

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
