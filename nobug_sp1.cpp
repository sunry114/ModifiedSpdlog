#include <iostream>
#include <vector>
#include <limits>
#include <utility>
#include <algorithm>
#include <time.h>
#include "../h/mylog.h"
#include "../../github/spdlog/include/spdlog/cpuusage.h"

// 定义图的边
struct Edge
{
    int from, to;
    int weight;
};

void BellmanFord(const std::vector<Edge> &edges, int V, int E, int src)
{
    // 初始化距离数组，源点到自身的距离为0，其他点为无穷大
    std::vector<int> dist(V, std::numeric_limits<int>::max());
    dist.at(src) = 0; // 使用 at 方法进行边界检查

    // 进行V-1次松弛操作
    for (int i = 0; i < V - 1; ++i)
    {
        for (const auto &edge : edges)
        {
            if (dist[edge.from] != std::numeric_limits<int>::max() &&
                dist[edge.from] + edge.weight < dist[edge.to])
            {
                dist[edge.to] = dist[edge.from] + edge.weight;
            }
        }
        printf("Relaxation count: %d times\n", i + 1);
    }

    // 检查负权回路
    for (const auto &edge : edges)
    {
        if (dist[edge.from] != std::numeric_limits<int>::max() &&
            dist[edge.from] + edge.weight < dist[edge.to])
        {
            printf("Graph contains negative weight cycle.\n");
            return; // 如果存在负权回路，则结束算法
        }
    }

    // 输出结果
    for (int i = 0; i < V; ++i)
    {
        std::cout << "Distance from vertex " << src << " to vertex " << i << " is " << dist[i] << std::endl;
    }
}

int main()
{
    clock_t start = clock();
    double rate[4];
    rate[0] = get_sysCpuUsage();
    std::cout << "K9 Replication Based On Spdlog." << std::endl
            << "--- Finding shortest path with Bellman-Ford Algorithm ---" << std::endl;
    try
    {
        // 示例图
        const int V = 5; // 顶点数
        const int E = 8; // 边数
        std::vector<Edge> edges = {
            {0, 1, -1}, {0, 2, 4}, {1, 2, 3}, {1, 3, 2}, {1, 4, 2}, {3, 2, 5}, {3, 1, 1}, {4, 3, -3}};
        printf("The edges have been assigned.");
        rate[1] = get_sysCpuUsage();
        int src = 0;
        BellmanFord(edges, V, E, src);
        rate[2] = get_sysCpuUsage();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Terminated. Exception occurred: " << e.what() << std::endl;
        rate[3] = get_sysCpuUsage();
    }
    double sum = 0, ave = 0;
    for (int j = 0; j < 4; j++)
    {
        sum = sum + rate[j];
    }
    ave = sum / 4;
    std::cout << "Average CPU rate: " << ave << "%" << std::endl;
    clock_t finish = clock();
    clock_t dur = finish - start;
    std::cout << "Runing time: " << dur << "ticks" << std::endl;
    return 0;
}