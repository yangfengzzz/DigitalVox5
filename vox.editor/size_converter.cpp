//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/size_converter.h"

#include <cmath>

namespace vox::editor {
std::pair<float, SizeConverter::SizeUnit> SizeConverter::ConvertToOptimalUnit(float p_value, SizeUnit p_unit) {
    if (p_value == 0.0f) return {0.0f, SizeUnit::BYTE};
    const float kBytes = Convert(p_value, p_unit, SizeUnit::BYTE);
    const int kDigits = static_cast<int>(trunc(log10(kBytes)));
    const auto kTargetUnit =
            static_cast<SizeUnit>(fmin(3.0f * floor(kDigits / 3.0f), static_cast<float>(SizeUnit::TERA_BYTE)));

    return {Convert(kBytes, SizeUnit::BYTE, kTargetUnit), kTargetUnit};
}

float SizeConverter::Convert(float p_value, SizeUnit p_from, SizeUnit p_to) {
    const float kFromValue = powf(1024.0f, static_cast<float>(p_from) / 3.0f);
    const float kToValue = powf(1024.0f, static_cast<float>(p_to) / 3.0f);

    return p_value * (kFromValue / kToValue);
}

std::string SizeConverter::UnitToString(SizeUnit p_unit) {
    switch (p_unit) {
        case SizeConverter::SizeUnit::BYTE:
            return "B";
        case SizeConverter::SizeUnit::KILO_BYTE:
            return "KB";
        case SizeConverter::SizeUnit::MEGA_BYTE:
            return "MB";
        case SizeConverter::SizeUnit::GIGA_BYTE:
            return "GB";
        case SizeConverter::SizeUnit::TERA_BYTE:
            return "TB";
    }

    return "?";
}

}  // namespace vox::editor
