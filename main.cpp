#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <omp.h>
#include <chrono>
#include "graph.h"
#include "queue.h"
#include "bitmap.h"
#include "bfs.h"

uint64_t timeSinceEpochMillisec() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("The arguments should have 3, but %d currently.\n", argc - 1);
        printf("Usage: ./bfs <graph_file> <output_file> <thread_num>\n");
        return 0;
    }

    printf("Progam start...\n");
    // get the input matrix filename.
    std::string infile_dir = argv[1];
    std::ifstream infile(infile_dir);
    // get the output filename.
    std::string outfile_dir = argv[2];
    // get the repeat num;
    int repeat_num = atoi(argv[3]);

    if (!infile.is_open()) {
        std::cerr << "failed to open the file " << infile_dir << "\n";
        return 0;
    }


    std::string line;
    std::vector<Edge> edgelist;
    printf("Starting to load edges...\n");
    int a, b, c;
    std::getline(infile, line, '\n');
    std::istringstream iss(line);
    iss >> a >> b >> c;
    while(!infile.eof()) {
        std::getline(infile, line, '\n');
        std::istringstream iss(line);
        iss >> a >> b >> c;
        edgelist.emplace_back(a, b);
    }

    Graph graph = Graph(edgelist);
    BFS bfs = BFS(graph, 16, 16);

    int test_thread_nums[] = {1, 2, 4, 8, 16, 24, 32, 48};
    for (int thread_num : test_thread_nums) {
        printf("---------- thread num: %d\n", thread_num);
        omp_set_num_threads(thread_num);
        uint64_t time_0 = timeSinceEpochMillisec();
        for (int k = 0; k < repeat_num; ++k) {
            printf("---------- iteration: %d\n", k);
            uint64_t time_1 = timeSinceEpochMillisec();
            int steps;
            if (thread_num == 1) {
                steps = bfs.bfsNaiive(2);
            } else {
                steps = bfs.bfsParallel(2);
            }
            uint64_t time_2 = timeSinceEpochMillisec();
            printf("---------- iteration: %d, time: %lld ms\n", k, time_2 - time_1);
            printf("           gMTEPS: %f\n", (float) steps / (float) (time_2 - time_1) * 1e-3);

        }
        uint64_t time_3 = timeSinceEpochMillisec();
        printf("---------- Average time: %f ms\n", (float)(time_3 - time_0) / (float)repeat_num);

        int y = 0;
        std::ofstream out(outfile_dir + "_parallel_" + std::to_string(thread_num) + "_thread.out");
        for (int i = 0; i < graph.get_num_node(); i++) {
            if (bfs.parent[i] >= 0) {
                y += 1;
                out << i << " " << bfs.distance[i] << " " << bfs.parent[i] << std::endl;
            }
        }
        out.close();
        uint64_t time_4 = timeSinceEpochMillisec();

        printf("---------- file saved with line: %d with time %lu ms.\n", y, (time_4 - time_3));
    }

    return 0;
}
