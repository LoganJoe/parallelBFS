//
// Created by Logan on 2022/11/17.
//

#ifndef BFS_BFS_H
#define BFS_BFS_H
#include<cstdint>
#include"graph.h"
#include"queue.h"
#include"bitmap.h"
#include<queue>


class BFS {
    Graph &graph;
    Bitmap *curr, *prev;
    SlidingQueue *queue;

    int64_t num_node, num_edge;
    int alpha, beta;

    int bottomUp(int &steps);

    int topDown(int &steps);

    void queueToBitmap(SlidingQueue *queue_, Bitmap *bitmap);

    void bitmapToQueue(Bitmap *bitmap, SlidingQueue *queue_);

public:
    Node* parent{};
    int64_t* distance{};

    BFS(Graph &graph, int alpha, int beta);

    ~BFS();

    void initialize();

    int bfsNaiive(Node root);

    int bfsParallel(Node root);


};


#endif //BFS_BFS_H
