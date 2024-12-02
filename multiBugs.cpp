#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <fstream>
#include <time.h>
#include "../h/mylog.h"
#include "../../github/spdlog/include/spdlog/cpuusage.h"

std::string file_path = "./log/multiBugs/";
LoggerSelector *p_selector = new LoggerSelector(file_path);
HrgLogger *my_multi_logger = p_selector->select_logger("both");
// 定义一个表示学生的类
class Student
{
public:
    std::string name;
    int age;
    float gpa;

    void readStudent()
    {
        // Bug 1: 忽略了剩余的输入缓冲区中的换行符
        std::cout << "Enter student name: ";
        std::cin >> name;
        std::cout << "Enter student age: ";
        std::cin >> age;
        std::cout << "Enter student GPA: ";
        std::cin >> gpa;
        if (gpa < 0 || gpa > 4.0)
        {
            my_multi_logger->print_error("Input invalid GPA!");
        }
        if (age < 0 || age > 100)
        {
            my_multi_logger->print_error("Input invalid age!");
        }
    }

    void printStudent() const
    {
        std::cout << "Name: " << name << ", Age: " << age << ", GPA: " << gpa << std::endl;
    }
};
// 分区函数
int partition(std::vector<int> &arr, int low, int high)
{
    // Bug 2: 选择最后一个元素作为基准
    int pivot = arr[high]; 
    int i = low - 1;
    for (int j = low; j < high; j++)
    {
        if (arr[j] < pivot)
        {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    my_multi_logger->print_debug("Swaped {} and {}", arr[i + 1], arr[high]);
    return i + 1;
}
// 快速排序算法
void quickSort(std::vector<int> &arr, int low, int high)
{
    if (low < high)
    {
        int pivot = partition(arr, low, high);
        quickSort(arr, low, pivot - 1);
        quickSort(arr, pivot + 1, high);
    }
}

// 二分搜索算法
int binarySearch(const std::vector<int> &arr, int target)
{
    // Bug 3: high 应该是 arr.size() - 1
    int low = 0, high = arr.size();

    my_multi_logger->print_debug("arr.size() = {}", high);
    while (low <= high)
    {
        int mid = (low + high) / 2;
        if (arr[mid] == target)
        {
            return mid;
        }
        else if (arr[mid] < target)
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }
    return -1;
}

// 动态分配内存并返回智能指针
std::shared_ptr<int> allocateMemory()
{
    // Bug 4: 未检查内存分配是否成功
    int *ptr = new int[10];
    return std::shared_ptr<int>(ptr);
}

// 释放内存
void freeMemory(std::shared_ptr<int> ptr)
{
    // Bug 5: 释放后未将指针置为 nullptr, 野指针
    delete[] ptr.get();
}

// 计算两个整数的和
int sum(int a, int b)
{
    if (a > 0 && b > 0 && a > std::numeric_limits<int>::max() - b)
    {
        my_multi_logger->print_error("Integer overflow detected!");
    }
    return a + b;
}

// 模拟文件读取
std::string readFile(const std::string &filename)
{
    // Bug 6: 未检查文件是否打开成功
    std::ifstream file(filename);
    // if (!file.is_open())
    // {
    //     my_multi_logger->print_error("Failed to open file!");
    // }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

// 模拟文件写入
void writeFile(const std::string &filename, const std::string &content)
{
    // Bug 7: 未检查文件是否打开成功
    std::ofstream file(filename);
    // if (!file.is_open())
    // {
    //     my_multi_logger->print_error("Failed to open file for writing");
    // }
    file << content;
}

int main()
{
    try
    {
        // 读取学生信息
        Student student;
        student.readStudent();
        student.printStudent();

        try
        {
            // 动态分配内存
            auto ptr = allocateMemory();
            freeMemory(ptr);
        }
        catch (const std::exception &e)
        {
            // 捕获所有标准异常
            my_multi_logger->print_error("Exception occurred: {}", e.what());
        }
        try
        {
            // 计算两个整数的和
            // Bug 8: 有意引入溢出错误
            int a = 2147483647; // INT_MAX
            int b = 1;
            int result = sum(a, b);
            my_multi_logger->print_debug("a = {}, b = {}", a, b);
        }
        catch (const std::exception &e)
        {
            // 捕获所有溢出异常
            my_multi_logger->print_error("Exception occurred: {}", e.what());
        }
        // 测试快速排序
        std::vector<int> arr = {5, 2, 9, 1, 5, 6};
        quickSort(arr, 0, arr.size() - 1);
        std::cout << "Sorted array: ";
        for (int num : arr)
        {
            std::cout << num << " ";
        }
        std::cout << std::endl;

        // 测试二分搜索
        int target = 5;
        int index = binarySearch(arr, target);
        if (index != -1)
        {
            std::cout << "Element found at index: " << index << std::endl;
        }
        else
        {
            std::cout << "Element not found" << std::endl;
        }

        // 读取文件
        std::string content = readFile("example.txt");
        std::cout << "File content: " << content << std::endl;

        // 写入文件
        writeFile("output.txt", "Hello, World!");
    }
    catch (const std::exception &e)
    {
        // 捕获所有标准异常
        my_multi_logger->print_error("Exception occurred: {}", e.what());
    }
    return 0;
}