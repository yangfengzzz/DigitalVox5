//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "file_logger.h"

VKBP_DISABLE_WARNINGS()
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
VKBP_ENABLE_WARNINGS()

namespace plugins {
FileLogger::FileLogger() :
FileLoggerTags("File Logger",
               "Enable log output to a file.",
               {}, {&log_file_flag}) {
}

bool FileLogger::is_active(const vox::CommandParser &parser) {
    return parser.contains(&log_file_flag);
}

void FileLogger::init(const vox::CommandParser &parser) {
    if (parser.contains(&log_file_flag)) {
        if (spdlog::default_logger()) {
            std::string log_file_name = parser.as<std::string>(&log_file_flag);
            spdlog::default_logger()->sinks().push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file_name, true));
        }
    }
}
}        // namespace plugins
