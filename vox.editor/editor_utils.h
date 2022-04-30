//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

namespace vox::editor {
std::string u_char_2_hex(unsigned char c);

struct Hash {
    Hash() = default;
    
    Hash(unsigned char *d, int l) {
        length = l;
        data = new unsigned char[length];
        std::memcpy(data, d, length);
    }
    
    ~Hash() = default;
    
    [[nodiscard]] std::string to_string() const {
        std::string hash;
        
        for (int i = 0; i < length; i++) {
            hash += u_char_2_hex(data[i]);
        }
        
        return hash;
    }
    
    int length = 0;
    unsigned char *data{};
};

#define MAX(x, y) ((x) > (y) ? (x) : (y))

#ifdef TERR3D_WIN32
std::wstring s2ws(const std::string &s);
#endif

void open_url(std::string url);

std::string show_save_file_dialog(std::string ext = ".terr3d");

std::string openfilename();

std::string show_open_file_dialog(std::string ext = "*.glsl");

std::string read_shader_source_file(std::string path, bool *result);

std::string get_executable_path();

std::string get_executable_dir();

std::string generate_id(uint32_t length);

std::string fetch_url(std::string base_url, std::string path);

char *u_char_2_char(unsigned char *data, int length);

bool file_exists(std::string path, bool write_access = false);

bool path_exist(const std::string &s);

bool is_net_work_connected();

char *read_binary_file(std::string path, int *size, uint32_t size_to_load = -1);

char *read_binary_file(std::string path, uint32_t size_to_load = -1);

Hash md_5_file(std::string path);

void download_file(std::string base_url, std::string url_path, std::string path, int size = -1);

void save_to_file(std::string filename, std::string content = "");

void log(const char *log);

void log(std::string log);

#ifdef TERR3D_WIN32
#include <windows.h>
void regSet(HKEY hkeyHive, const char *pszVar, const char *pszValue);
#endif

void accoc_file_type();

void mk_dir(std::string path);

void copy_file_data(std::string source, std::string destination);

bool delete_file_t(std::string path);

bool is_key_down(int key);

bool is_mouse_button_down(int button);

void show_message_box(std::string message, std::string title = "Info");

void toggle_system_console();

}
