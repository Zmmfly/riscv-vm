#ifndef __ZMMFLY_RV32_F_H__
#define __ZMMFLY_RV32_F_H__

#include "zmmfly/rv32_base.h"

namespace zmmfly::rv32::v1
{

class F
: public inst_intf_t
{

public:
    F();

    std::string name();

    rv_err_t execute_normal(uint32_t inst, registers_t& regs, bus_t& bus, inst_map_t& inst_map);
};

};

#endif /* __ZMMFLY_RV32_F_H__ */
