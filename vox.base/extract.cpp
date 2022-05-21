//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "extract.h"

#include <unordered_map>

#include "extract_zip.h"
#include "file_system.h"
#include "logging.h"

namespace vox::utility {

static const std::unordered_map<std::string, std::function<void(const std::string&, const std::string&)>>
        file_extension_to_extract_function{
                {"zip", ExtractFromZIP},
        };

void Extract(const std::string& file_path, const std::string& extract_dir) {
    const std::string format = utility::filesystem::GetFileExtensionInLowerCase(file_path);
    LOGI("Extracting {}.", file_path)

    if (file_extension_to_extract_function.count(format) == 0) {
        LOGE("Extraction Failed: unknown file extension for "
             "{} (format: {}).",
             file_path, format)
    }

    file_extension_to_extract_function.at(format)(file_path, extract_dir);
    LOGI("Extracted to {}.", extract_dir)
}

}  // namespace vox::utility
