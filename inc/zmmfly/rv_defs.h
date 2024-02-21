#ifndef __ZMMFLY_RV_DEFS_H__
#define __ZMMFLY_RV_DEFS_H__

#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>
#include <algorithm>
#include <any>
#include <map>
#include <unordered_map>
#include <tuple>
#include <vector>
#include <functional>

namespace zmmfly
{

using rv_err_t = enum rv_err_t
{
    RV_EOK = 0,
    RV_EUNALIGNED,
    RV_EUNSUPPORTED,
    RV_EOVERLAP,
    RV_EOVERFLOW,
    RV_ERANGE,
    RV_ETYPE,
    RV_EBUSFAULT,
    RV_EIO,
    RV_ENOTFOUND,
};

};

#endif /* __ZMMFLY_RV_DEFS_H__ */
