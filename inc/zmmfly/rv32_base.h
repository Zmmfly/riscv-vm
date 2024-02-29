#ifndef __ZMMFLY_RV32_INST_H__
#define __ZMMFLY_RV32_INST_H__

#include <cstdint>
#include "zmmfly/rv_intf.h"
#include "zmmfly/rv_busmgr.hpp"

namespace zmmfly::rv32
{

#pragma pack(push, 1)

using bus_t       = zmmfly::rv::bus_mgr_intf<uint32_t>;
using bus_intf_t  = zmmfly::rv::bus_intf<uint32_t>;
using inst_intf_t = zmmfly::rv::inst_intf<uint32_t>;
#pragma pack(pop)

};

#endif /* __ZMMFLY_RV32_INST_H__ */
