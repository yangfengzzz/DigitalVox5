//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <json/json.h>

#include <fstream>

#include "../ijson_convertible_io.h"
#include "logging.h"

namespace vox {

namespace {
using namespace io;

bool ReadIJsonConvertibleFromJSONStream(std::istream &json_stream, utility::IJsonConvertible &object) {
    Json::Value root_object;
    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    Json::String errs;
    bool is_parse_successful = parseFromStream(builder, json_stream, &root_object, &errs);
    if (!is_parse_successful) {
        LOGW("Read JSON failed: {}.", errs)
        return false;
    }
    return object.ConvertFromJsonValue(root_object);
}

bool WriteIJsonConvertibleToJSONStream(std::ostream &json_stream, const utility::IJsonConvertible &object) {
    Json::Value root_object;
    if (!object.ConvertToJsonValue(root_object)) {
        return false;
    }
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "\t";
    auto writer = builder.newStreamWriter();
    writer->write(root_object, &json_stream);
    return true;
}

}  // unnamed namespace

namespace io {

bool ReadIJsonConvertibleFromJSON(const std::string &filename, utility::IJsonConvertible &object) {
    std::ifstream file_in(filename);
    if (!file_in.is_open()) {
        LOGW("Read JSON failed: unable to open file: {}", filename)
        return false;
    }
    bool success = ReadIJsonConvertibleFromJSONStream(file_in, object);
    file_in.close();
    return success;
}

bool WriteIJsonConvertibleToJSON(const std::string &filename, const utility::IJsonConvertible &object) {
    std::ofstream file_out(filename);
    if (!file_out.is_open()) {
        LOGW("Write JSON failed: unable to open file: {}", filename)
        return false;
    }
    bool success = WriteIJsonConvertibleToJSONStream(file_out, object);
    file_out.close();
    return success;
}

bool ReadIJsonConvertibleFromJSONString(const std::string &json_string, utility::IJsonConvertible &object) {
    std::istringstream iss(json_string);
    return ReadIJsonConvertibleFromJSONStream(iss, object);
}

bool WriteIJsonConvertibleToJSONString(std::string &json_string, const utility::IJsonConvertible &object) {
    std::ostringstream oss;
    bool success = WriteIJsonConvertibleToJSONStream(oss, object);
    json_string = oss.str();
    return success;
}

}  // namespace io
}  // namespace vox
