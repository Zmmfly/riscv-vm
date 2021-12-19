#ifndef __XUTILS_STRING_H__
#define __XUTILS_STRING_H__

#include <string>
#include <list>

namespace xutils
{
    std::list<std::string> split(const std::string &s, const std::string delimiter);
    bool is_hexstr(std::string &str);
    bool is_dischar(char c);
    uint8_t ascii2byte(const char ascii);
    bool hex2bin(std::string str, uint8_t *&out, size_t max_len);
}

#endif