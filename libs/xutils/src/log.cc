#include "xutils/string.h"
#include "xutils/log.h"
#include <string.h>
#include <iomanip>
#include <stdarg.h>

namespace xutils{

log::log()
{}

log::~log()
{}

bool log::set_logfile(std::string logfile)
{
    m_logfile.open(logfile, std::ios::out | std::ios::app);
    return m_logfile.is_open();
}

void log::set_stdout(bool stdout_en, bool stderr_en)
{
    m_stdout_set = stdout_en;
    m_stderr_set = stderr_en;
}

void log::set_callback(log::log_callback_t fn)
{
    m_log_callback = fn;
}

void log::log_v(std::string tag, std::string fmt, va_list args, int level)
{
    std::stringstream ss;
    if (level == LOG_LEVEL_INFO)
    {
        ss << "\033[32mI ";
    }
    else if (level == LOG_LEVEL_WARN)
    {
        ss << "\033[33mW ";
    }
    else if (level == LOG_LEVEL_ERROR)
    {
        ss << "\033[31mE ";
    }
    else if (level == LOG_LEVEL_FATAL)
    {
        ss << "\033[91mF ";
    }
    else if (level == LOG_LEVEL_DEBUG)
    {
        ss << "\033[35mD ";
    }
    else
    {
        ss << "\033[95m? ";
    }

    ss << "(" << tag << ") " << fmt;
    std::string str = ss.str();
    int len = str.length();
    char *buf = new char[len + 1];
    memset(buf, 0, len + 1);
    vsnprintf(buf, len + 1, str.c_str(), args);
    std::string str_log = buf;
    str_log += "\033[0m\n";
    delete[] buf;

    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_logfile.is_open())
    {
        m_logfile << str_log;
    }

    if (m_stdout_set)
    {
        if (level != LOG_LEVEL_ERROR && level != LOG_LEVEL_FATAL)
        {
            std::cout << str_log;
        }
        else if (m_stderr_set)
        {
            std::cerr << str_log;
        }
    }
    else if (m_stderr_set)
    {
        if (level == LOG_LEVEL_ERROR && level == LOG_LEVEL_FATAL)
        {
            std::cerr << str_log;
        }
    }

    if (m_log_callback)
    {
        m_log_callback(level, tag, str_log, args);
    }
}

void log::I(std::string tag, std::string fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_v(tag, fmt, args, LOG_LEVEL_INFO);
    va_end(args);
}

void log::W(std::string tag, std::string fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_v(tag, fmt, args, LOG_LEVEL_WARN);
    va_end(args);
}

void log::E(std::string tag, std::string fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_v(tag, fmt, args, LOG_LEVEL_ERROR);
    va_end(args);
}

void log::F(std::string tag, std::string fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_v(tag, fmt, args, LOG_LEVEL_FATAL);
    va_end(args);
}

void log::D(std::string tag, std::string fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_v(tag, fmt, args, LOG_LEVEL_DEBUG);
    va_end(args);
}

void log::HEX(std::string tag, void *buf, size_t len)
{
    std::stringstream ss;
    std::stringstream si;
    std::stringstream sx;
    std::string sa;

    ss << "\033[35mHEX (" << tag << ") \n";
    for (size_t i = 0; i < len; i++)
    {
        if (i % 16 == 0)
        {
            si.str("");
            si << std::hex << std::setw(8) << std::setfill('0') << i;
            si << ": ";

            sx.str("");
            sa = "|";
        }

        sx << std::hex << std::setw(2) << std::setfill('0') << (int)((unsigned char *)buf)[i] << " ";
        sa += std::string(1, is_dischar(((char *)buf)[i]) ? ((char *)buf)[i] : '.');

        if (i % 16 == 15)
        {
            sa += "\n";
            ss << si.str() << sx.str() << sa;
        }
        else if (i % 8 == 7)
        {
            sx << " ";
            sa += " ";
        }

    }

    ss << "\033[0m\n";
    std::string str_log = ss.str();

    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_logfile.is_open())
    {
        m_logfile << str_log;
    }

    if (m_stdout_set)
    {
        std::cout << str_log;
    }

    if (m_log_callback)
    {
        m_log_callback(LOG_LEVEL_DEBUG, tag, str_log, NULL);
    }
}

}
