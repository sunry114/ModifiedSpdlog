#include <iostream>
#include <vector>
#include <time.h>
#include "../h/mylog.h"
#include "../../github/spdlog/include/spdlog/cpuusage.h"

// 创建一个输出到标准输出的彩色日志记录器
std::string file_path = "./log/sort/";
LoggerSelector *p_selector = new LoggerSelector(file_path);
HrgLogger *my_multi_logger = p_selector->select_logger("both");

// 冒泡排序函数
void bubbleSort(std::vector<int> &arr)
{
    int n = arr.size();
    bool swapped;

    for (int i = 0; i < n - 1; i++)
    {
        swapped = false;
        // 故意引入的 bug，应该为 j < n - 1 - i
        for (int j = 0; j < n - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                my_multi_logger->print_debug("Swapping {} and {}", arr[j], arr[j + 1]);
                std::swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        if (!swapped)
        {
            break;
        }
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
    if (!my_multi_logger)
    {
        std::cout << "Create multi logger failed!" << std::endl;
        exit(1);
    }
    // 测试自动生成日志名称
    my_multi_logger->generate_file_name_automaticaly();
    my_multi_logger->set_print_level(LOG_LEVEL_DEBUG);
    // 测试数据
    std::vector<int> data = {5, 3, 8, 4, 2, 9, 1, 6, 7};

    // 调用冒泡排序
    bubbleSort(data);
    rate[0] = get_sysCpuUsage();
    my_multi_logger->print_info("CPU rate: {} %", rate[0]);

    // 打印排序后的数据
    my_multi_logger->print_info("{}: ", "Sorted array");
    for (int num : data)
    {
        my_multi_logger->print_info("{}", num);
        rate[num] = get_sysCpuUsage();
    }

    // 检查排序结果
    if (isSorted(data))
    {
        my_multi_logger->print_info("Array is sorted correctly.");
    }
    else
    {
        my_multi_logger->print_error("Array is not sorted correctly.");
    }
    rate[10] = get_sysCpuUsage();
    my_multi_logger->print_info("CPU rate: {} %", rate[10]);
    double sum = 0, ave = 0;
    for (int j = 0; j < 11; j++)
    {
        sum = sum + rate[j];
    }
    ave = sum / data.size();
    my_multi_logger->print_info("Average CPU rate: {} %", ave);
    clock_t finish = clock();
    clock_t dur = finish - start;
    my_multi_logger->print_info("Runing time: {} ticks", dur);
    return 0;
}