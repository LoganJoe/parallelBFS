# CS121 Parallel Computing Lab 1 Breadth First Search using OpenMP

Hongyang Lin, 2020533180

### Configuration 

The benchmark tested on the following machine:

- Supermicro AS-4124GS-TNR
- AMD EPYC 7742 64-Core Processor @ 3.2GHz (64 Cores / 64 Threads) * 2
- 1TB ECC memory
- Ubuntu 20.04.3 5.15.0-52-generic
- building with gcc version 9.4.0 (Ubuntu 9.4.0-1ubuntu1~20.04.1)
- cmake version 3.16.3
- -O3 optimization.

### How to run
run the following command to compile the program:
```bash
sudo apt install -y build-essential git libomp cmake
cd $PROJECT_DIRECTORY
mkdir build
cd build
cmake ../
make
./main <input_file> <output_file> <repeat_times>
```

### Memory Locality

Frist we need to settle a data structure to store all the data. At first I used the Chained Adjacency Table to store the graph, but we found that jumping over the all memory whil cause a bad cache hit rate. After searching some data structures including the sparse matrix discussed in the lecture, I found that Compressed Sparse Row (CSR) format are great to solve the problem with high memory locality. The code can be found in `src/graph.cpp`.

### Parallel BFS

First I read serval papers talk about the ways of parallel bfs. https://crd.lbl.gov/assets/pubs_presos/mtaapbottomup2D.pdf gave out Parallel 2D top-down BFS algorithm and Parallel 2D bottom-up BFS algorithm that I found efficient to parallel using openmp. 

The top-down approach is very similar to the traditional approach in that we maintain a queue and place each expanded node in a buffer and add it to the queue when the current node has finished expanding. The bottom-up approach scans each unvisited vertex, searches its neighbouring vertices, adds the node to the next layer if one exists, and continues to consider other nodes.

These two methods address each other's shortcomings, but still have their advantages and disadvantages. I reference the implentation of a benchmark suite in https://github.com/sbeamer/gapbs/blob/master/src/bfs.cc, they mixed the top-down approach and bottom-up approach to structure a hyper-bfs to get a better performance. 

The code can be found in `src/bfs.cpp`.

### Optimization

#### Atomic operation

I For data that needs to be accessed simultaneously, atomic operations can avoid the performance overhead of locking. I therefore use a large number of atomic operations to reduce overhead. like`__sync_bool_compare_and_swap`, `__sync_fetch_and_add` and `__sync_bool_compare_and_swap`.  To be menctioned that some atomic operation need to introduce a refercnce instead of a pointer. Which can be referneced in https://gcc.gnu.org/onlinedocs/gcc/_005f_005fsync-Builtins.html. 

#### Bitmap 
To store a visited[] in bottom-up bfs, we need a structure to store it efficiently. following the implementation in https://github.com/sbeamer/gapbs, I develop an atomic bitmap to check whether the node are visited or not. The core code should be like this:
```c++
void set_bit_atomic(size_t pos) {
    uint64_t old_val, new_val;
    do {
      old_val = start_[word_offset(pos)];
      new_val = old_val | ((uint64_t) 1l << bit_offset(pos));
    } while (!compare_and_swap(start_[word_offset(pos)], old_val, new_val));
  }
```
The code can be found in `src/bfs.cpp`.

#### SlidingQueue
As everytime we should add and read a chunk of data in the queue, to get better parallelism, we use the sliding queue to store the data thats need to be accessed. Different from the usual queue, I give two tails that separately point to the end of this level and the end of the whole queue. 
To get better performance, In the top-down approach, we can directly access the whole level in rawdata, with the data parallelism by openmp, we can separate the data in chunks and get better performance. The code can be found in `src/queue.cpp`.

#### BufferQueue
As multiple thread write the queue at a same time will cause a worse performance, I introducted the buffer of each thread, that will temporily store the data in their own buffer. It will flush the buffer and write the data into the main queue if the scan finished or the buffer is full. Everytime we directly copy the buffer into the queue, which will significantly reduce the junk of the queue push operation. The code can be found in `src/queue.cpp`.

A heuristic can be used to switch between bottom-up and top-down bfs to achieve the best of both worlds. We follow the authors' suggestion on parameter tuning.  Note some of the results might be inaccurate, since the graphs may be directed and the heuristic method will have the different result about the parents.

### Problem

I also test the serval algorithms of parallel BFS program. I benefit much in the open source repo https://github.com/onesuper/bfs_in_parallel. One optimization is to get less lock makes competition more fierce, and since the platform we configured have two sockets and totally 8 NUMA, too severe fierce may cause too severe traffic in the crosslink bus, and higher latency may reduce the performance.

More I referenced the https://github.com/orancanoren/GPU-CPU-Parallel-Graph-Algorithms.

And we found that the current algorithm still have much overhead in the small scale data. Like we shown below, web-Stanford will have worst acclerate rate.

### Benchmark method
I test our algorithm with the thread {1, 2, 4, 8, 16, 24, 32, 48, 64, 96, 128}. Every case should run in multiple times to avoid errors. The raw result will be placed in the `./output` directory.






