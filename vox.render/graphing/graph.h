//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#include <json.hpp>

#include "error.h"
#include "graph_node.h"

namespace vox {
namespace graphing {
struct Edge {
  size_t id;
  size_t from;
  size_t to;
  nlohmann::json options;
  Edge(size_t id_, size_t s, size_t t) :
	  id(id_),
	  from(s),
	  to(t) {};
};

/**
 * @brief Graph is an implementation of an adjacency list graph. The nodes are created from a variadic function and their implementation is defined by the given NodeType
 */
class Graph {
 public:
  Graph(const char *name);

  static const size_t node_not_found = 0;

  /**
   * @brief Create a new style
   *
   * @param style_name the group name
   * @param color the hex color of the group
   */
  void new_style(std::string style_name, std::string color);

  /**
   * @brief Create a node object
   *
   * @param title of node
   * @param style corresponds to the key used when using new_type(style, color)
   * @param data json data to be displayed with node
   * @return size_t id of node
   */
  size_t create_node(const char *title = "Node", const char *style = NULL, const nlohmann::json &data = {}) {
	  size_t id = new_id();
	  nodes[id] = std::make_unique<Node>(id, title, style, data);
	  return id;
  }

  /**
   * @brief Find a node from a reference
   * 		  If the node does not exist then the reference will be node_not_found
   *
   * @param name of node
   * @return size_t if of node
   */
  size_t find_ref(std::string &name);

  /**
   * @brief Add a readable reference to a node
   *
   * @param name of reference
   * @param id of node
   */
  void add_ref(std::string &name, size_t id);

  /**
   * @brief Remove a readable reference to a node
   *
   * @param name of ref
   */
  void remove_ref(std::string &name);

  /**
   * @brief Add an edge to the graph
   * @param from source node
   * @param to target node
   */
  void add_edge(size_t from, size_t to);

  /**
   * @brief Remove Edge from the graph
   * @param from source node
   * @param to target node
   */
  void remove_edge(size_t from, size_t to);

  /**
   * @brief Dump the graphs state to json in the given file name
   * @param file_name to dump to
   */
  bool dump_to_file(std::string file_name);

  size_t new_id();

 private:
  size_t next_id = 1;
  std::vector<Edge> adj;
  std::unordered_map<size_t, std::unique_ptr<Node>> nodes;
  std::unordered_map<std::string, size_t> refs;
  std::string name;
  std::unordered_map<std::string, std::string> style_colors;
};
}        // namespace graphing
}        // namespace vox
