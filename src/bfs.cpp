//
// Created by Logan on 2022/11/17.
//

#include "bfs.h"

BFS::BFS(Graph &graph, int alpha, int beta) : graph(graph) {
    queue = new SlidingQueue(graph.num_node);
    curr = new Bitmap(graph.num_node);
    prev = new Bitmap(graph.num_node);

    num_node = graph.num_node;
    num_edge = graph.num_edge;

    parent = new Node[num_node];
    distance = new int64_t[num_node];

    this->alpha = alpha;
    this->beta = beta;
}

BFS::~BFS() {
    delete[] parent;
    delete[] distance;
    delete queue;
    delete curr;
    delete prev;
}

void BFS::initialize() {
#pragma omp parallel for schedule(static) default(none)
    for (int i = 0; i < num_node; ++i) {
        parent[i] = graph.get_out_degree(i) == 0 ? -1 : -graph.get_out_degree(i);
        distance[i] = -1;
    }
}

void BFS::queueToBitmap(SlidingQueue *queue_, Bitmap *bitmap) {
#pragma omp parallel for schedule(guided) default(none) shared(queue_, bitmap)
    for (auto i = queue_->begin(); i < queue_->end(); ++i) {
        Node n = *i;
        bitmap->setBitAtomic(n);
    }
}

void BFS::bitmapToQueue(Bitmap *bitmap, SlidingQueue *queue_) {
#pragma omp parallel default(none) shared(bitmap, queue_)
    {
        BufferredQueue bufferedQueue(*queue_);
#pragma omp for
        for (Node i = 0; i < num_node; ++i)
            if (bitmap->checkBit(i)) bufferedQueue.push_back(i);
        bufferedQueue.flush();
    }
    queue_->slide();
}

int BFS::bfsNaiive(Node root) {
    int num_visited = 0;
    std::queue<int> q;
    q.push(root);
    initialize();
    distance[root] = 0;
    parent[root] = root;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (Node* i = graph.get_index_u(u); i < graph.get_index_u(u + 1); ++i) {
            Node v = *i;
            if (distance[v] == -1) {
                distance[v] = distance[u] + 1;
                parent[v] = u;
                q.push(v);
                num_visited++;
            }
        }
    }
    return num_visited;
}

int BFS::topDown(int &steps) {
    int64_t num_visited = 0;
#pragma omp parallel default(none) shared(num_visited, steps)
    {
        BufferredQueue bufferedQueue(*queue);
#pragma omp for reduction(+: num_visited, steps) schedule(dynamic)
        for (auto it = queue->begin(); it < queue->end(); it++) {
            Node u = *it;
            for (auto i = graph.get_index_u(u); i < graph.get_index_u(u + 1); ++i) {
                steps++;
                Node v = *i;
                Node w = parent[v];
                if (w < 0 && __sync_bool_compare_and_swap(&parent[v], w, u)) {
                    distance[v] = distance[u] + 1;
                    bufferedQueue.push_back(v);
                    num_visited += -w;
                }
            }
        }
        bufferedQueue.flush();
    }
    return num_visited;
}

int BFS::bottomUp(int &steps) {
    int64_t num_visited = 0;
    curr->reset();
#pragma omp parallel for reduction(+: num_visited, steps) schedule(dynamic) default(none)
    for (Node u = 0; u < num_node; ++u) {
        if (parent[u] < 0) {
            for (auto i = graph.get_index_v(u); i < graph.get_index_v(u + 1); ++i) {
                steps++;
                Node v = *i;
                if (prev->checkBit(v)) {
                    parent[u] = v;
                    distance[u] = distance[v] + 1;
                    num_visited++;
                    curr->setBit(u);
                    break;
                }
            }
        }
    }
    return num_visited;
}


int BFS::bfsParallel(Node root) {
    initialize();
    int64_t num_visited = 0, edge_unvisited = num_edge;
    parent[root] = root, distance[root] = 0;
    queue->clear();
    queue->push_back(root);
    queue->slide();
    prev->reset(), curr->reset();

    int outnum = graph.get_out_degree(root);
    int step = 0;

    while(!queue->empty()) {
        if (outnum > edge_unvisited / alpha) {
            queueToBitmap(queue, prev);
            int prm, rm = queue->size();
            queue->slide();
            do {
                prm = rm;
                rm = bottomUp(step);
                prev->swap(curr);
            } while (rm >= prm || rm > num_node / beta);
            bitmapToQueue(prev, queue);
            num_visited = 1;
        } else {
            edge_unvisited -= num_visited;
            num_visited = topDown(step);
            queue->slide();
        }
    }
    return step;

}

