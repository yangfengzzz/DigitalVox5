//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "graph.h"

#include "platform/filesystem.h"

namespace vox::graphing {
Graph::Graph(const char *new_name) :
name_(new_name) {
}

void Graph::new_style(const std::string &style_name, const std::string &color) {
    auto it = style_colors_.find(style_name);
    if (it != style_colors_.end()) {
        it->second = color;
    } else {
        style_colors_.insert({style_name, color});
    }
}

size_t Graph::new_id() {
    return next_id_++;
}

size_t Graph::find_ref(std::string &name) {
    auto it = refs_.find(name);
    if (it == refs_.end()) {
        return node_not_found_;
    }
    return it->second;
}

void Graph::add_ref(std::string &name, size_t id) {
    refs_.insert({name, id});
}

void Graph::remove_ref(std::string &name) {
    auto it = refs_.find(name);
    if (it != refs_.end()) {
        refs_.erase(it);
    }
}

void Graph::add_edge(size_t from, size_t to) {
    auto it = std::find_if(adj_.begin(), adj_.end(), [from, to](auto &e) -> bool { return e.from == from && e.to == to; });
    if (it == adj_.end()) {
        adj_.emplace_back(new_id(), from, to);
    }
}

void Graph::remove_edge(size_t from, size_t to) {
    auto it = std::find_if(adj_.begin(), adj_.end(), [from, to](auto &e) -> bool { return e.from == from && e.to == to; });
    if (it != adj_.end()) {
        adj_.erase(it);
    }
}

bool Graph::dump_to_file(const std::string &file) {
    std::vector<nlohmann::json> edges;
    for (auto &e : adj_) {
        auto it = nodes_.find(e.from);
        if (it != nodes_.end()) {
            e.options["style"] = it->second->attributes_["style"];
        }
        e.options["id"] = e.id;
        e.options["source"] = e.from;
        e.options["target"] = e.to;
        edges.push_back({{"data", e.options}});
    }
    
    std::vector<nlohmann::json> node_json;
    auto it = nodes_.begin();
    while (it != nodes_.end()) {
        node_json.push_back(it->second->attributes_);
        it++;
    }
    
    nlohmann::json j = {
        {"name", name_},
        {"nodes", node_json},
        {"edges", edges},
        {"styles", style_colors_}};
    
    return fs::WriteJson(j, file);
}

}        // namespace vox
