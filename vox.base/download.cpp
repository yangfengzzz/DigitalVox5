//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "download.h"

// clang-format off
// Must include openssl before curl to build on Windows.
#include <openssl/md5.h>

// https://stackoverflow.com/a/41873190/1255535
#ifdef WINDOWS
#pragma comment(lib, "wldap32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "Ws2_32.lib")
#define USE_SSLEAY
#define USE_OPENSSL
#endif

#define CURL_STATICLIB

#include <curl/curl.h>
#include <curl/easy.h>
// clang-format on

#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

#include "dataset.h"
#include "file_system.h"
#include "logging.h"

namespace vox::utility {

std::string GetMD5(const std::string& file_path) {
    if (!utility::filesystem::FileExists(file_path)) {
        LOGE("{} does not exist.", file_path)
    }

    std::ifstream fp(file_path.c_str(), std::ios::in | std::ios::binary);

    if (!fp.good()) {
        std::ostringstream os;
        LOGE("Cannot open {}", file_path)
    }

    constexpr const std::size_t buffer_size{1 << 12};  // 4 KiB
    char buffer[buffer_size];
    unsigned char hash[MD5_DIGEST_LENGTH] = {0};

    MD5_CTX ctx;
    MD5_Init(&ctx);

    while (fp.good()) {
        fp.read(buffer, buffer_size);
        MD5_Update(&ctx, buffer, fp.gcount());
    }

    MD5_Final(hash, &ctx);
    fp.close();

    std::ostringstream os;
    os << std::hex << std::setfill('0');

    for (unsigned char i : hash) {
        os << std::setw(2) << static_cast<unsigned int>(i);
    }

    return os.str();
}

static size_t WriteDataCb(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

std::string DownloadFromURL(const std::string& url,
                            const std::string& md5,
                            const std::string& prefix,
                            const std::string& data_root) {
    // Always print URL to inform the user. If the download fails, the user
    // knows the URL.
    LOGI("Downloading {}", url)

    // Sanity checks.
    if (md5.size() != MD5_DIGEST_LENGTH * 2) {
        LOGE("Invalid md5 length {}, expected to be {}.", md5.size(), MD5_DIGEST_LENGTH * 2)
    }
    if (prefix.empty()) {
        LOGE("Download prefix cannot be empty.")
    }

    // Resolve path.
    const std::string resolved_data_root = data_root.empty() ? data::LocateDataRoot() : data_root;
    const std::string file_dir = resolved_data_root + "/" + prefix;
    const std::string file_name = utility::filesystem::GetFileNameWithoutDirectory(url);
    const std::string file_path = file_dir + "/" + file_name;
    if (!utility::filesystem::DirectoryExists(file_dir)) {
        utility::filesystem::MakeDirectoryHierarchy(file_dir);
    }

    // Check if the file exists.
    if (utility::filesystem::FileExists(file_path) && GetMD5(file_path) == md5) {
        LOGI("{} exists and md5 matches. Skipped downloading.", file_path)
        return file_path;
    }

    // Download.
    CURL* curl;
    FILE* fp;
    CURLcode res;
    curl = curl_easy_init();
    if (!curl) {
        LOGE("Failed to initialize CURL.")
    }
    fp = fopen(file_path.c_str(), "wb");
    if (!fp) {
        LOGE("Failed to open file {}.", file_path)
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);  // -L redirection.
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteDataCb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    fclose(fp);

    if (res == CURLE_OK) {
        const std::string actual_md5 = GetMD5(file_path);
        if (actual_md5 == md5) {
            LOGI("Downloaded to {}", file_path)
        } else {
            LOGE("MD5 mismatch for {}.\n- Expected: {}\n- Actual  : {}", file_path, md5, actual_md5)
        }
    } else {
        LOGE("Download failed with error code: {}.", curl_easy_strerror(res))
    }

    return file_path;
}

std::string DownloadFromURL(const std::vector<std::string>& mirror_urls,
                            const std::string& md5,
                            const std::string& prefix,
                            const std::string& data_root) {
    for (const auto& mirror_url : mirror_urls) {
        try {
            return DownloadFromURL(mirror_url, md5, prefix, data_root);
        } catch (const std::exception& ex) {
            LOGW("Failed to download from {}. Exception {}.", mirror_url, ex.what())
        }
    }
    throw std::runtime_error("Downloading failed from available mirrors.");
}

}  // namespace vox::utility
