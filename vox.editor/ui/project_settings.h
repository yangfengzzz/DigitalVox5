//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.editor/ini_file.h"
#include "vox.render/ui/widgets/panel_transformables/panel_window.h"
#include "vox.render/ui/widgets/texts/text.h"

namespace vox {
using namespace ui;

namespace editor::ui {
class ProjectSettings : public PanelWindow {
public:
    /**
     * Constructor
     * @param title title
     * @param opened opened
     * @param window_settings windowSettings
     */
    ProjectSettings(const std::string &title,
                    bool opened,
                    const PanelWindowSettings &window_settings,
                    const std::string &project_path,
                    const std::string &project_name);

    /**
     * Generate a gatherer that will get the value associated to the given key
     * @param key_name keyName
     */
    template <typename T>
    std::function<T()> GenerateGatherer(const std::string &key_name) {
        return std::bind(&fs::IniFile::Get<T>, &project_settings_, key_name);
    }

    /**
     * Generate a provider that will set the value associated to the given key
     * @param key_name keyName
     */
    template <typename T>
    std::function<void(T)> GenerateProvider(const std::string &key_name) {
        return std::bind(&fs::IniFile::Set<T>, &project_settings_, key_name, std::placeholders::_1);
    }

public:
    /**
     * Reset project settings ini file
     */
    void ResetProjectSettings();

    /**
     * Verify that project settings are complete (No missing key).
     * Returns true if the integrity is verified
     */
    bool IsProjectSettingsIntegrityVerified();

    /**
     * Apply project settings to the ini file
     */
    void ApplyProjectSettings();

    fs::IniFile &ProjectSettingFiles();

private:
    fs::IniFile project_settings_;
};

}  // namespace editor::ui
}  // namespace vox
