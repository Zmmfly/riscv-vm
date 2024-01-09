#ifndef __ZMMFLY_RV_DEFS_H__
#define __ZMMFLY_RV_DEFS_H__

#include <cstdint>
#include <memory>

namespace zmmfly
{

using rv_err_t = enum rv_err_t
{
    RV_EOK          = 0,
    RV_EUNALIGNED   = 1,
    RV_EUNSUPPORTED = 2,
    RV_EOVERLAP     = 3,
};

};

#endif /* __ZMMFLY_RV_DEFS_H__ */
