//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cassert>
#include "ini_file.h"

namespace vox::fs {
template<typename T>
inline T IniFile::get(const std::string &p_key) {
    if constexpr (std::is_same<bool, T>::value) {
        if (!is_key_existing(p_key))
            return false;
        
        return string_to_boolean(data_[p_key]);
    } else if constexpr (std::is_same<std::string, T>::value) {
        if (!is_key_existing(p_key))
            return std::string("NULL");
        
        return data_[p_key];
    } else if constexpr (std::is_integral<T>::value) {
        if (!is_key_existing(p_key))
            return static_cast<T>(0);
        
        return static_cast<T>(std::atoi(data_[p_key].c_str()));
    } else if constexpr (std::is_floating_point<T>::value) {
        if (!is_key_existing(p_key))
            return static_cast<T>(0.0f);
        
        return static_cast<T>(std::atof(data_[p_key].c_str()));
    } else {
        // static_assert(false, "The given type must be : bool, integral, floating point or string");
        return T();
    }
}

template<typename T>
inline T IniFile::get_or_default(const std::string &p_key, T p_default) {
    return is_key_existing(p_key) ? get<T>(p_key) : p_default;
}

template<typename T>
inline bool IniFile::set(const std::string &p_key, const T &p_value) {
    if (is_key_existing(p_key)) {
        if constexpr (std::is_same<bool, T>::value) {
            data_[p_key] = p_value ? "true" : "false";
        } else if constexpr (std::is_same<std::string, T>::value) {
            data_[p_key] = p_value;
        } else if constexpr (std::is_integral<T>::value) {
            data_[p_key] = std::to_string(p_value);
        } else if constexpr (std::is_floating_point<T>::value) {
            data_[p_key] = std::to_string(p_value);
        } else {
            // static_assert(false, "The given type must be : bool, integral, floating point or string");
        }
        
        return true;
    }
    
    return false;
}

template<typename T>
inline bool IniFile::add(const std::string &p_key, const T &p_value) {
    if (!is_key_existing(p_key)) {
        if constexpr (std::is_same<bool, T>::value) {
            register_pair(p_key, p_value ? "true" : "false");
        } else if constexpr (std::is_same<std::string, T>::value) {
            register_pair(p_key, p_value);
        } else if constexpr (std::is_integral<T>::value) {
            register_pair(p_key, std::to_string(p_value));
        } else if constexpr (std::is_floating_point<T>::value) {
            register_pair(p_key, std::to_string(p_value));
        } else {
            // static_assert(false, "The given type must be : bool, integral, floating point or std::string");
        }
        
        return true;
    }
    
    return false;
}

}
