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

/**
 * @brief Convert hex string to binary
 * 
 * @param str 
 * @param out 
 * @param max_len If input zero, will alloc memory for out
 * @return true 
 * @return false 
 */
bool hex2bin(std::string str, uint8_t *&out, size_t max_len)
{
    uint8_t *buf   = nullptr;
    size_t  hexlen = 0;

    if (!is_hexstr(str))return false;
    hexlen = max_len > 0 ? max_len : str.size();
    buf    = max_len > 0 ? out : (uint8_t *)(new uint8_t[hexlen]);

    //TODO complete this function
}

}