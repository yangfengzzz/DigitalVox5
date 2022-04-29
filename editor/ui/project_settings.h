//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "ui/widgets/texts/text.h"
#include "ui/widgets/panel_transformables/panel_window.h"
#include "ini_file.h"

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
    ProjectSettings(const std::string &title, bool opened,
                    const PanelWindowSettings &window_settings,
                    const std::string &project_path, const std::string &project_name);
    
    /**
     * Generate a gatherer that will get the value associated to the given key
     * @param key_name keyName
     */
    template<typename T>
    std::function<T()> generate_gatherer(const std::string &key_name) {
        return std::bind(&fs::IniFile::get<T>, &project_settings_, key_name);
    }
    
    /**
     * Generate a provider that will set the value associated to the given key
     * @param key_name keyName
     */
    template<typename T>
    std::function<void(T)> generate_provider(const std::string &key_name) {
        return std::bind(&fs::IniFile::set<T>, &project_settings_, key_name, std::placeholders::_1);
    }
    
public:
    /**
     * Reset project settings ini file
     */
    void reset_project_settings();
    
    /**
     * Verify that project settings are complete (No missing key).
     * Returns true if the integrity is verified
     */
    bool is_project_settings_integrity_verified();
    
    /**
     * Apply project settings to the ini file
     */
    void apply_project_settings();
    
    fs::IniFile &project_settings();
    
private:
    fs::IniFile project_settings_;
};

}
}
