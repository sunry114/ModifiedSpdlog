#include <iostream>
#include <vector>
#include <limits>
#include <utility>
#include <algorithm>
#include <time.h>
#include "../h/mylog.h"
#include "../../github/spdlog/include/spdlog/cpuusage.h"

std::string file_path = "./log/sp1/";
LoggerSelector *p_selector = new LoggerSelector(file_path);
HrgLogger *my_multi_logger = p_selector->select_logger("both");

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
        my_multi_logger->print_debug("Relaxation count: {} times", i + 1);
    }

    // 检查负权回路
    for (const auto &edge : edges)
    {
        if (dist[edge.from] != std::numeric_limits<int>::max() &&
            dist[edge.from] + edge.weight < dist[edge.to])
        {
            my_multi_logger->print_error("Graph contains negative weight cycle");
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
    if (!my_multi_logger)
    {
        std::cout << "Create multi logger failed!" << std::endl;
        exit(1);
    }
    // 测试自动生成日志名称
    my_multi_logger->generate_file_name_automaticaly();
    my_multi_logger->set_print_level(LOG_LEVEL_DEBUG);
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
        my_multi_logger->print_info("The edges have been assigned.");
        rate[1] = get_sysCpuUsage();
        // 故意引入的bug：源点设置为-1，这将导致数组越界访问
        int src = 0;
        BellmanFord(edges, V, E, src);
        rate[2] = get_sysCpuUsage();
    }
    catch (const std::exception &e)
    {
        my_multi_logger->print_error("Terminated. Exception occurred: {}. ", e.what());
        rate[3] = get_sysCpuUsage();
    }
    double sum = 0, ave = 0;
    for (int j = 0; j < 4; j++)
    {
        sum = sum + rate[j];
    }
    ave = sum / 4;
    my_multi_logger->print_info("Average CPU rate: {} %", ave);
    clock_t finish = clock();
    clock_t dur = finish - start;
    my_multi_logger->print_info("Runing time: {} ticks", dur);
    return 0;
}