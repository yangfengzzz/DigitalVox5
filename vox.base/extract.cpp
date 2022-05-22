//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.base/extract.h"

#include <unordered_map>

#include "vox.base/extract_zip.h"
#include "vox.base/file_system.h"
#include "vox.base/logging.h"

namespace vox::utility {

static const std::unordered_map<std::string, std::function<void(const std::string&, const std::string&)>>
        kFileExtensionToExtractFunction{
                {"zip", ExtractFromZIP},
        };

void Extract(const std::string& file_path, const std::string& extract_dir) {
    const std::string kFormat = utility::filesystem::GetFileExtensionInLowerCase(file_path);
    LOGI("Extracting {}.", file_path)

    if (kFileExtensionToExtractFunction.count(kFormat) == 0) {
        LOGE("Extraction Failed: unknown file extension for "
             "{} (format: {}).",
             file_path, kFormat)
    }

    kFileExtensionToExtractFunction.at(kFormat)(file_path, extract_dir);
    LOGI("Extracted to {}.", extract_dir)
}

}  // namespace vox::utility
