#ifndef __XUTILS_LOG_H__
#define __XUTILS_LOG_H__

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include <mutex>

namespace xutils{

class log
{
    public:
        typedef std::function<void(int level, std::string tag, std::string fmt, va_list args)> log_callback_t;
        typedef enum log_level_t
        {
            LOG_LEVEL_INFO = 1,
            LOG_LEVEL_WARN,
            LOG_LEVEL_ERROR,
            LOG_LEVEL_FATAL,
            LOG_LEVEL_DEBUG,
        } log_level_t;

        log();
        ~log();
        bool set_logfile(std::string logfile);
        void set_stdout(bool stdout_en, bool stderr_en);
        void set_callback(log_callback_t fn);

    private:
        void log_v(std::string tag, std::string fmt, va_list args, int level);

    public:
        void I(std::string tag, std::string fmt, ...);
        void W(std::string tag, std::string fmt, ...);
        void E(std::string tag, std::string fmt, ...);
        void F(std::string tag, std::string fmt, ...);
        void D(std::string tag, std::string fmt, ...);
        void HEX(std::string tag, void *buf, size_t len);

    private:
        bool m_stdout_set  = true;
        bool m_stderr_set  = true;

        std::mutex m_mutex;
        std::fstream m_logfile;
        log_callback_t m_log_callback;
};

};

#endif