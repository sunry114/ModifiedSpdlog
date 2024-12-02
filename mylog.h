#ifndef __HRG_LOG__
#define __HRG_LOG__

#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/logger.h"

#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/common.h"

enum LoggerMode
{
    LAB_MODE = 0,   // 实验室模式，打印比较细致的信息
    TRIAL_MODE = 1, // 试用模式，打印调试信息
    USER_MODE = 2,  // 用户模式，打印较重要的信息
};

#define LOG_MODE USER_MODE

using namespace spdlog;

#define LOG_LEVEL_TRACE spdlog::level::trace
#define LOG_LEVEL_DEBUG spdlog::level::debug
#define LOG_LEVEL_INFO spdlog::level::info
#define LOG_LEVEL_WARN spdlog::level::warn
#define LOG_LEVEL_ERROR spdlog::level::err
#define LOG_LEVEL_CRITICAL spdlog::level::critical
#define LOG_LEVEL_OFF spdlog::level::off

using print_level = spdlog::level::level_enum; // 变量重命名
using format_string = string_view_t;           // 变量重命名 //wstring_view_t;   //fmt::basic_string_view<char>;

class HrgLogger
{
public:
    HrgLogger()
    {
        logger_created = false;
        logger_droped = false;
    }

    ~HrgLogger();

    std::shared_ptr<spdlog::logger> hrg_logger;

    bool logger_created;       // 标识日志创建状态
    bool logger_droped;        // 标识日志关闭状态
    time_t logger_create_time; // 日志创建时间

    /* Generate log file name automatically according to real time, usually used when many real-time log files should be output */
    virtual void generate_file_name_automaticaly();

    /* Set a specified log file name, usually used when only one log file is needed.
        input param @filename is the specified filename.
      */
    void set_specified_file_name(std::string filename);

    /* The abstract api for create logger, would be realized by the derived classes */
    virtual void create_logger() = 0; // 创建logger的纯虚函数，只能在子函数中实现

    /* 设置日志的打印级别 */
    void set_print_level(print_level lvl)
    {
        hrg_logger->set_level(lvl);
    }

    /* 重新封装logger的trace, debug, warn, error和critical打印接口 */
    template <typename... Args>
    inline void print_trace(format_string fmt, const Args &...args)
    {
        hrg_logger->trace(fmt, args...);
    }

    template <typename... Args>
    inline void print_debug(format_string fmt, const Args &...args)
    {
        hrg_logger->debug(fmt, args...);
    }

    template <typename... Args>
    inline void print_info(format_string fmt, const Args &...args)
    {
        hrg_logger->info(fmt, args...);
    }

    template <typename... Args>
    inline void print_warn(format_string fmt, const Args &...args)
    {
        hrg_logger->warn(fmt, args...);
    }

    template <typename... Args>
    inline void print_error(format_string fmt, const Args &...args)
    {
        hrg_logger->error(fmt, args...);
    }

    template <typename... Args>
    inline void print_critical(format_string fmt, const Args &...args)
    {
        hrg_logger->critical(fmt, args...);
    }

    /* 销毁logger */
    void destroy_logger()
    {
        spdlog::drop_all();
        logger_droped = true;
    }

protected:
    std::string log_full_name; // The full name contains log_path and log_file_name
    std::string log_path;      // The log path which is specified in class constructor
};

/* 控制台logger子类 */
class ConsoleLogger : public HrgLogger
{
public:
    ConsoleLogger()
    {
        std::cout << "ConsoleLogger constructor." << std::endl;
    }

    void generate_file_name_automaticaly() {} // 对于控制台子类来说，无需生成file文件，因此该方法的函数体置空

    virtual void create_logger();
};

/* 文件logger子类 */
class FileLogger : public HrgLogger
{
public:
    FileLogger(std::string str)
    {
        log_path = str;
        std::cout << "FileLogger Constructor." << std::endl;
    }

    virtual void create_logger();
};

/* 控制台+文件复合logger子类 */
class MultiLogger : public HrgLogger
{
public:
    MultiLogger(std::string str)
    {
        log_path = str;
        std::cout << "MultiLogger with parameters constructor." << std::endl;
    }

    virtual void create_logger();
};

/* Logger选择类，通过传入的logger模式生成相应的logger */
class LoggerSelector
{
public:
    LoggerSelector() {}
    LoggerSelector(std::string str)
    {
        path = str;
    }
    ~LoggerSelector() {}

    HrgLogger *select_logger(std::string out_type);
    HrgLogger *select_logger(int mode);

private:
    std::string path;
};

#endif
HrgLogger::~HrgLogger()
{
    destroy_logger();
}

void HrgLogger::generate_file_name_automaticaly()
{
    struct tm *cur_time;
    time_t local_time;
    time(&local_time);
    cur_time = localtime(&local_time);

    /* 通过时间命名日志文件 */
    char filename[200];
    strftime(filename, 100, "%Y-%m-%d_%H-%M-%S.log", cur_time);

    /* log file name, does not contain file path */
    std::string log_file_name = std::string(filename);

    /* 判断日志路径是否存在，若不存在，则创建 */
    /* Check if the input log path exists, if not, create the path */
    if (access(log_path.c_str(), F_OK) != 0)
    {
        mkdir(log_path.c_str(), S_IRWXU);
    }

    /* 日志文件全路径 */
    log_full_name = log_path + log_file_name;

    /* 记录日志创建时间，在通过时间长度控制日志文件大小时，该成员变量会被使用 */
    logger_create_time = local_time;
}

/* 除了使用时间命名日志文件，用户还可以自定义日志文件名称 */
void HrgLogger::set_specified_file_name(std::string filename)
{
    /* Check if the input log path exists, if not, create the path */
    if (access(log_path.c_str(), F_OK) != 0)
    {
        mkdir(log_path.c_str(), S_IRWXU);
    }

    /* Make full log path */
    log_full_name = log_path + filename;
}

/* 创建控制台logger */
void ConsoleLogger::create_logger()
{
    try
    {
        hrg_logger = spdlog::stdout_color_mt("console");
        // hrg_logger->set_pattern("%+");
        hrg_logger->set_pattern("[%Y-%m-%d %T][thread %t][%l]%v");
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Create console logger failed: " << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!hrg_logger)
    {
        std::cout << "Create console logger failed." << std::endl;
    }
    else
    {
        std::cout << "Create console logger seccessfully." << std::endl;
        logger_created = true;
    }
}

/* 创建文件logger */
void FileLogger::create_logger()
{
    try
    {
        hrg_logger = spdlog::basic_logger_mt("basic_logger", log_full_name.c_str());

        spdlog::set_pattern("[%Y-%m-%d %T][%l]%v");
        hrg_logger->set_level(spdlog::level::trace);
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Create file logger failed: " << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!hrg_logger)
    {
        std::cout << " Create file logger failed." << std::endl;
    }
    else
    {
        std::cout << "Create file logger successfully." << std::endl;
        logger_created = true;
    }
}

/* 创建复合logger */
void MultiLogger::create_logger()
{
    try
    {
        /* 通过multi-sink的方式创建复合logger，实现方式为：先分别创建文件sink和控制台sink，并将两者放入sink 向量中，组成一个复合logger */
        /* file sink */
        std::cout << "MultiLogger: log_full_name = " << log_full_name << std::endl;
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_full_name.c_str(), true);
        file_sink->set_level(spdlog::level::trace);
        file_sink->set_pattern("[%Y-%m-%d %T][%l]%v");
        std::cout << "MultiLogger: create file sink OK." << std::endl;

        /* 控制台sink */
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
        console_sink->set_pattern("%+");
        std::cout << "MultiLogger: create console sink OK." << std::endl;

        /* Sink组合 */
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(console_sink);
        sinks.push_back(file_sink);
        hrg_logger = std::make_shared<spdlog::logger>("multi-sink", begin(sinks), end(sinks));

        std::cout << "MultiLogger: create multi sink OK." << std::endl;
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Create multi-logger failed: " << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!hrg_logger)
    {
        std::cout << " Create multi-logger failed." << std::endl;
    }
    else
    {
        std::cout << "Create multi-logger successfully." << std::endl;
        logger_created = true;
    }
}

/* 通过指定输出名称选择logger */
HrgLogger *LoggerSelector::select_logger(std::string out_type)
{
    HrgLogger *p_logger = NULL;

    if (out_type == "console")
    {
        p_logger = new ConsoleLogger();
        p_logger->create_logger();
    }
    else if (out_type == "file")
    {
        p_logger = new FileLogger(path);
        std::cout << "For file logger, path = " << path << std::endl;
        p_logger->generate_file_name_automaticaly();
        p_logger->create_logger();
    }
    else if (out_type == "both")
    {
        p_logger = new MultiLogger(path);
        p_logger->generate_file_name_automaticaly();
        p_logger->create_logger();
    }
    else
    {
        std::cout << "Unsupported logger type!" << std::endl;
        return NULL;
    }

    return p_logger;
}

/* 通过日志模式选择logger */
HrgLogger *LoggerSelector::select_logger(int mode)
{
    HrgLogger *p_logger = NULL;

    switch (mode)
    {
    case LAB_MODE:
    {
        p_logger = new ConsoleLogger();
        p_logger->create_logger();
        p_logger->set_print_level(LOG_LEVEL_TRACE);

        break;
    }
    case TRIAL_MODE:
    {
        p_logger = new MultiLogger(path);
        p_logger->generate_file_name_automaticaly();
        p_logger->create_logger();
        p_logger->set_print_level(LOG_LEVEL_DEBUG);

        break;
    }
    case USER_MODE:
    {
        p_logger = new FileLogger(path);
        p_logger->generate_file_name_automaticaly();
        p_logger->create_logger();
        p_logger->set_print_level(LOG_LEVEL_INFO);

        break;
    }
    default:
    {
        break;
    }
    }

    return p_logger;
}
