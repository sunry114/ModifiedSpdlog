#include <iostream>
#include <vector>
#include <time.h>
#include "../h/mylog.h"
#include "../../github/spdlog/include/spdlog/cpuusage.h"

// 冒泡排序函数
void bubbleSort(std::vector<int> &arr)
{
    int n = arr.size();
    bool swapped;

    for (int i = 0; i < n - 1; i++)
    {
        swapped = false;
        for (int j = 0; j < n - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                std::swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        if (!swapped)
            break;
    }
}

// 辅助函数：检查数组是否已排序
bool isSorted(const std::vector<int> &arr)
{
    for (size_t i = 0; i < arr.size() - 1; ++i)
    {
        if (arr[i] > arr[i + 1])
        {
            return false;
        }
    }
    return true;
}

int main()
{
    clock_t start = clock();
    double rate[11];
    std::cout << "K9 Replication Based On Spdlog." << std::endl
            << "--- Bubble Sort ---" << std::endl;
    // 测试数据
    std::vector<int> data = {5, 3, 8, 4, 2, 9, 1, 6, 7};
    // 调用冒泡排序
    bubbleSort(data);
    rate[0] = get_sysCpuUsage();
    // 打印排序后的数据
    for (int num : data)
    {
        rate[num] = get_sysCpuUsage();
    }
    // 检查排序结果
    if (isSorted(data))
        std::cout << "Array is sorted correctly." << std::endl;
    else
        std::cout << "Array not sorted correctly." << std::endl;
    rate[10] = get_sysCpuUsage();
    double sum = 0, ave = 0;
    for (int j = 0; j < 11; j++)
    {
        sum = sum + rate[j];
    }
    ave = sum / data.size();
    std::cout << "Average CPU rate: " << ave << "%" << std::endl;
    clock_t finish = clock();
    clock_t dur = finish - start;
    std::cout << "Runing time: " << dur << "ticks" << std::endl;
    return 0;
}