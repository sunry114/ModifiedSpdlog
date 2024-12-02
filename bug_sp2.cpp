#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <utility>
#include <time.h>
#include "../h/mylog.h"
#include "../../github/spdlog/include/spdlog/cpuusage.h"

std::string file_path = "./log/sp2/";
LoggerSelector *p_selector = new LoggerSelector(file_path);
HrgLogger *my_multi_logger = p_selector->select_logger("both");
// 定义图的边
struct Edge
{
    int to;
    int weight;
};

// 定义优先队列中的节点
struct Node
{
    int vertex;
    int distance;
    bool operator>(const Node &other) const
    {
        return distance > other.distance;
    }
};

void Dijkstra(const std::vector<std::vector<Edge>> &graph, int V, int src)
{
    if (src < 0 || src >= V)
    {
        throw std::out_of_range("Source vertex is out of range");
    }
    // 初始化距离数组，源点到自身的距离为0，其他点为无穷大
    std::vector<int> dist(V, std::numeric_limits<int>::max());
    dist[src] = 0;

    // 优先队列，存储 (顶点, 距离) 对
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
    pq.push({src, 0});
    my_multi_logger->print_info("The vertexes and distances are stored. ");
    while (!pq.empty())
    {
        Node top = pq.top();
        pq.pop();

        int u = top.vertex;
        int d = top.distance;

        // 如果当前距离大于已知距离，跳过
        if (d > dist[u])
            continue;

        // 遍历邻接顶点
        for (const auto &edge : graph[u])
        {
            int v = edge.to;
            int weight = edge.weight;

            // 更新距离
            if (dist[u] + weight < dist[v])
            {
                dist[v] = dist[u] + weight;
                pq.push({v, dist[v]});
                my_multi_logger->print_debug("Update distance from {} to {}", dist[u], dist[v]);
            }
        }
    }
    my_multi_logger->print_info("The result has been printed in consel.");
    // 输出结果
    for (int i = 0; i < V; ++i)
    {
        std::cout << "Distance from vertex " << src << " to vertex " << i << " is " << dist[i] << std::endl;
    }
}

int main()
{
    clock_t start = clock();
    double rate[5];
    if (!my_multi_logger)
    {
        std::cout << "Create multi logger failed!" << std::endl;
        exit(1);
    }
    // 测试自动生成日志名称
    my_multi_logger->generate_file_name_automaticaly();
    rate[0] = get_sysCpuUsage();
    my_multi_logger->set_print_level(LOG_LEVEL_DEBUG);
    rate[1] = get_sysCpuUsage();
    std::cout << "K9 Replication Based On Spdlog." << std::endl
            << "--- Finding shortest path with Dijkstra Algorithm --" << std::endl;
    try
    {
        // 示例图
        const int V = 5; // 顶点数
        std::vector<std::vector<Edge>> graph(V);
        graph[0].push_back({1, 1});
        graph[0].push_back({2, 4});
        graph[1].push_back({2, 3});
        graph[1].push_back({3, 2});
        graph[1].push_back({4, 2});
        graph[3].push_back({2, 5});
        graph[3].push_back({1, 1});
        graph[4].push_back({3, -3}); // 注意这里有一个负权重边
        my_multi_logger->print_info("Vertexes and distances hven been initialized and assigned.");
        rate[2] = get_sysCpuUsage();
        // 故意引入的错误：源点设置为-1，这将导致数组越界访问
        int src = 0;
        Dijkstra(graph, V, src);
        rate[3] = get_sysCpuUsage();
    }
    catch (const std::out_of_range &e)
    {
        // 捕获 out_of_range 异常
        my_multi_logger->print_error("Terminated. Exception occurred: {}. ", e.what());
        rate[4] = get_sysCpuUsage();
    }
    // catch (const std::exception &e)
    // {
    //     // 捕获其他标准异常
    //     my_multi_logger->print_error("Terminated. Exception occurred: {}. ", e.what());
    //     rate[4] = get_sysCpuUsage();
    // }
    double sum = 0, ave = 0;
    for (int j = 0; j < 5; j++)
    {
        sum = sum + rate[j];
    }
    ave = sum / 5;
    my_multi_logger->print_info("Average CPU rate: {} %", ave);
    clock_t finish = clock();
    clock_t dur = finish - start;
    my_multi_logger->print_info("Runing time: {} ticks", dur);
    return 0;
}