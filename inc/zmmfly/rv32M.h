#ifndef __ZMMFLY_RV32_M_H__
#define __ZMMFLY_RV32_M_H__

#include "zmmfly/rv32_base.h"
#include <functional>

namespace zmmfly::rv32
{

class M
: public inst_intf_t
{

public:
    M();

    std::string name();

    rv_err_t execute_normal(uint32_t inst, registers_t& regs, bus_t& bus, inst_map_t& inst_map);
};

};

#endif /* __ZMMFLY_RV32_M_H__ */
