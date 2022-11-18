//
// Created by Logan on 2022/11/16.
//

#include "graph.h"


Graph::Graph(std::vector <Edge> &edge) {
    Node num_node_ = 0;
#pragma omp parallel for reduction(max: num_node_) default(none) shared(edge)
    for (auto e: edge) {
        num_node_ = std::max(num_node_, std::max(e.first, e.second));
    }
    num_node = num_node_ + 1;
    num_edge = edge.size();

    list_u = new Node[num_edge];
    list_v = new Node[num_edge];
    index_u = new Node *[num_node + 1];
    index_v = new Node *[num_node + 1];

    Node *degree_u = new Node[num_node];
    Node *degree_v = new Node[num_node];
#pragma omp parallel for default(none) shared(degree_u, degree_v)
    for (int i = 0; i < num_node; ++i) degree_u[i] = degree_v[i] = 0;

#pragma omp parallel for default(none) shared(edge, degree_u, degree_v)
    for (auto e: edge) {
        __sync_fetch_and_add(&degree_u[e.first], 1);
        __sync_fetch_and_add(&degree_v[e.second], 1);
    }

    int64_t *offset_u = new int64_t[num_node + 1];
    int64_t *offset_v = new int64_t[num_node + 1];

    offset_u[0] = offset_v[0] = 0;
    for (int i = 0; i < num_node; ++i) {
        offset_u[i + 1] = offset_u[i] + degree_u[i];
        offset_v[i + 1] = offset_v[i] + degree_v[i];
    }

#pragma omp parallel for default(none) shared(edge, list_u, list_v, offset_u, offset_v)
    for (int i = 0; i <= num_node; ++i) {
        index_u[i] = list_u + offset_u[i];
        index_v[i] = list_v + offset_v[i];
    }

    for (auto e: edge) {
        list_u[__sync_fetch_and_add(&offset_u[e.first], 1)] = e.second;
        list_v[__sync_fetch_and_add(&offset_v[e.second], 1)] = e.first;
    }

    delete[] degree_u;
    delete[] degree_v;

    delete[] offset_u;
    delete[] offset_v;
}

Graph::~Graph() {
    delete[] list_u;
    delete[] list_v;
    delete[] index_u;
    delete[] index_v;
}

int Graph::get_num_node() const {
    return num_node;
}

int Graph::get_num_edge() const {
    return num_edge;
}

Node Graph::get_out_degree(Node u) const {
    return index_u[u + 1] - index_u[u];
}

Node Graph::get_in_degree(Node u) const {
    return index_v[u + 1] - index_v[u];
}

Node *Graph::get_index_u(Node u) const {
    return index_u[u];
}

Node *Graph::get_index_v(Node u) const {
    return index_v[u];
}

