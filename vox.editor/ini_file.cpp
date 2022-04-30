//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "ini_file.h"
#include <filesystem>
#include <fstream>
#include <utility>
#include <vector>

namespace vox::fs {
IniFile::IniFile(std::string p_file_path) : file_path_(std::move(p_file_path)) {
    load();
}

void IniFile::reload() {
    remove_all();
    load();
}

bool IniFile::remove(const std::string &p_key) {
    if (is_key_existing(p_key)) {
        data_.erase(p_key);
        return true;
    }
    
    return false;
}

void IniFile::remove_all() {
    data_.clear();
}

bool IniFile::is_key_existing(const std::string &p_key) const {
    return data_.find(p_key) != data_.end();
}

void IniFile::register_pair(const std::string &p_key, const std::string &p_value) {
    register_pair(std::make_pair(p_key, p_value));
}

void IniFile::register_pair(const AttributePair &p_pair) {
    data_.insert(p_pair);
}

std::vector<std::string> IniFile::get_formatted_content() const {
    std::vector<std::string> result;
    
    for (const auto &[kEy, kValue] : data_)
        result.push_back(kEy + "=" + kValue);
    
    return result;
}

void IniFile::load() {
    std::fstream ini_file;
    ini_file.open(file_path_);
    
    if (ini_file.is_open()) {
        std::string current_line;
        
        while (std::getline(ini_file, current_line)) {
            if (is_valid_line(current_line)) {
                current_line.erase(std::remove_if(current_line.begin(), current_line.end(), isspace), current_line.end());
                register_pair(extract_key_and_value(current_line));
            }
        }
        
        ini_file.close();
    }
}

void IniFile::rewrite() const {
    std::ofstream outfile;
    outfile.open(file_path_, std::ios_base::trunc);
    
    if (outfile.is_open()) {
        for (const auto &[kEy, kValue] : data_)
            outfile << kEy << "=" << kValue << std::endl;
    }
    
    outfile.close();
}

std::pair<std::string, std::string> IniFile::extract_key_and_value(const std::string &p_attribute_line) {
    std::string key;
    std::string value;
    
    std::string *current_buffer = &key;
    
    for (auto &c : p_attribute_line) {
        if (c == '=')
            current_buffer = &value;
        else
            current_buffer->push_back(c);
    }
    
    return std::make_pair(key, value);
}

bool IniFile::is_valid_line(const std::string &p_attribute_line) {
    if (p_attribute_line.empty())
        return false;
    
    if (p_attribute_line[0] == '#' || p_attribute_line[0] == ';' || p_attribute_line[0] == '[')
        return false;
    
    if (std::count(p_attribute_line.begin(), p_attribute_line.end(), '=') != 1)
        return false;
    
    return true;
}

bool IniFile::string_to_boolean(const std::string &p_value) {
    return (p_value == "1" || p_value == "T" || p_value == "t" || p_value == "True" || p_value == "true");
}

}
