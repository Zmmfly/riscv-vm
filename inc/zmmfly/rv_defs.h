#ifndef __ZMMFLY_RV_DEFS_H__
#define __ZMMFLY_RV_DEFS_H__

#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>
#include <algorithm>
#include <any>
#include <map>
#include <tuple>
#include <vector>

namespace zmmfly
{

using rv_err_t = enum rv_err_t
{
    RV_EOK          = 0,
    RV_EUNALIGNED   = 1,
    RV_EUNSUPPORTED = 2,
    RV_EOVERLAP     = 3,
    RV_ETYPE        = 4,
    RV_EBUSFAULT    = 5,
    RV_EIO
};

};

#endif /* __ZMMFLY_RV_DEFS_H__ */
