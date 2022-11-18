//
// Created by Logan on 2022/11/16.
//

#ifndef BFS_GRAPH_H
#define BFS_GRAPH_H

#include <cstdint>
#include<vector>
#include <algorithm>

typedef int32_t Node;
typedef std::pair<Node, Node> Edge;

/*
 * Store the graph in CSR format.
 */
class Graph {
private:
    Node *list_u, *list_v;
    Node ** index_u, ** index_v;

    int64_t num_edge;

    int get_out_degree(Node u) const;

    int get_in_degree(Node u) const;

    Node *get_index_u(Node u) const;

    Node *get_index_v(Node u) const;

    friend class BFS;

public:
    ~Graph();

    Graph (std::vector<Edge> &edge);

    int64_t num_node;

    int get_num_node() const;

    int get_num_edge() const;
};

#endif //BFS_GRAPH_H
