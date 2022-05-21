//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

namespace vox::editor {
std::string UChar2Hex(unsigned char c);

struct Hash {
    Hash() = default;

    Hash(unsigned char *d, int l) {
        length = l;
        data = new unsigned char[length];
        std::memcpy(data, d, length);
    }

    ~Hash() = default;

    [[nodiscard]] std::string ToString() const {
        std::string hash;

        for (int i = 0; i < length; i++) {
            hash += UChar2Hex(data[i]);
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

void OpenUrl(std::string url);

std::string ShowSaveFileDialog(std::string ext = ".terr3d");

std::string Openfilename();

std::string ShowOpenFileDialog(std::string ext = "*.glsl");

std::string ReadShaderSourceFile(std::string path, bool *result);

std::string GetExecutablePath();

std::string GetExecutableDir();

std::string GenerateId(uint32_t length);

std::string FetchUrl(std::string base_url, std::string path);

char *UChar2Char(unsigned char *data, int length);

bool FileExists(std::string path, bool write_access = false);

bool PathExist(const std::string &s);

bool IsNetWorkConnected();

char *ReadBinaryFile(std::string path, int *size, uint32_t size_to_load = -1);

char *ReadBinaryFile(std::string path, uint32_t size_to_load = -1);

Hash Md5File(std::string path);

void DownloadFile(std::string base_url, std::string url_path, std::string path, int size = -1);

void SaveToFile(std::string filename, std::string content = "");

void Log(const char *log);

void Log(std::string log);

#ifdef TERR3D_WIN32
#include <windows.h>
void regSet(HKEY hkeyHive, const char *pszVar, const char *pszValue);
#endif

void AccocFileType();

void MkDir(std::string path);

void CopyFileData(std::string source, std::string destination);

bool DeleteFileT(std::string path);

bool IsKeyDown(int key);

bool IsMouseButtonDown(int button);

void ShowMessageBox(std::string message, std::string title = "Info");

void ToggleSystemConsole();

}  // namespace vox::editor
