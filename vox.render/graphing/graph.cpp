//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "graph.h"

#include "platform/filesystem.h"

namespace vox {
namespace graphing {
Graph::Graph(const char *new_name) :
	name(new_name) {
}

void Graph::new_style(std::string style_name, std::string color) {
	auto it = style_colors.find(style_name);
	if (it != style_colors.end()) {
		it->second = color;
	} else {
		style_colors.insert({style_name, color});
	}
}

size_t Graph::new_id() {
	return next_id++;
}

size_t Graph::find_ref(std::string &name) {
	auto it = refs.find(name);
	if (it == refs.end()) {
		return node_not_found;
	}
	return it->second;
}

void Graph::add_ref(std::string &name, size_t id) {
	refs.insert({name, id});
}

void Graph::remove_ref(std::string &name) {
	auto it = refs.find(name);
	if (it != refs.end()) {
		refs.erase(it);
	}
}

void Graph::add_edge(size_t from, size_t to) {
	auto it = std::find_if(adj.begin(), adj.end(), [from, to](auto &e) -> bool { return e.from == from && e.to == to; });
	if (it == adj.end()) {
		adj.push_back({new_id(), from, to});
	}
}

void Graph::remove_edge(size_t from, size_t to) {
	auto it = std::find_if(adj.begin(), adj.end(), [from, to](auto &e) -> bool { return e.from == from && e.to == to; });
	if (it != adj.end()) {
		adj.erase(it);
	}
}

bool Graph::dump_to_file(std::string file) {
	std::vector<nlohmann::json> edges;
	for (auto &e : adj) {
		auto it = nodes.find(e.from);
		if (it != nodes.end()) {
			e.options["style"] = it->second->attributes["style"];
		}
		e.options["id"] = e.id;
		e.options["source"] = e.from;
		e.options["target"] = e.to;
		edges.push_back({{"data", e.options}});
	}

	std::vector<nlohmann::json> node_json;
	auto it = nodes.begin();
	while (it != nodes.end()) {
		node_json.push_back(it->second->attributes);
		it++;
	}

	nlohmann::json j = {
		{"name", name},
		{"nodes", node_json},
		{"edges", edges},
		{"styles", style_colors}};

	return fs::write_json(j, file);
}        // namespace graphing

}        // namespace graphing
}        // namespace vox
