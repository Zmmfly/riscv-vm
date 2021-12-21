#include "xutils/string.h"

namespace xutils{

std::list<std::string> split(const std::string &s, const std::string delimiter)
{
    std::list<std::string> out;
    size_t pos_last = 0, pos_cur  = 0;
    do{
        pos_cur = s.find(delimiter, pos_last);
        out.push_back(s.substr(pos_last, pos_cur-pos_last-1));
        pos_last = pos_cur+delimiter.size();
    }while(pos_cur != std::string::npos);
    return out;
}

bool is_hexstr(std::string &str)
{
    if (str.size() % 2) return false;
    for(auto it : str) {
        if ( (it >= '0' && it <= '9') ) continue;
        if ( (it >= 'A' && it <= 'F') ) continue;
        if ( (it >= 'a' && it <= 'f') ) continue;
        return false;
    }
    return true;
}

bool is_dischar(char c)
{
    return ( c >= ' ' && c <= '~' ) ? true : false;
}

uint8_t ascii2byte(const char ascii)
{
    if (ascii >= '0' && ascii <= '9') return ascii - '0';
    if (ascii >= 'A' && ascii <= 'F') return ascii - 'A' + 10;
    if (ascii >= 'a' && ascii <= 'f') return ascii - 'a' + 10;
    return 0;
}

/**
 * @brief Convert hex string to binary
 * 
 * @param str 
 * @param out 
 * @param max_len If input zero, will alloc memory for out, need free by delete
 * @return true 
 * @return false 
 */
bool hex2bin(std::string str, uint8_t *&out, size_t max_len)
{
    uint8_t *buf   = nullptr;
    size_t  hexlen = 0;

    if (out == nullptr && max_len != 0) return false;

    if (!is_hexstr(str))return false;
    hexlen = max_len > 0 ? max_len : str.size();
    buf    = max_len > 0 ? out : (new uint8_t[hexlen]);

    for (size_t i=0; i<hexlen; i++)
    {
        buf[i] = ascii2byte(str[i*2]) << 4 | ascii2byte(str[i*2+1]);
    }
    out = buf;
    return true;
}

}
