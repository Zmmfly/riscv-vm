#ifndef __XUTILS_LOG_H__
#define __XUTILS_LOG_H__

#include <string>
#include <iostream>
#include <fstream>

namespace xutils{

class log
{
    public:
        log();
        log(std::string logfile_path);

    private:
        std::fstream m_logfile;
};

};

#endif