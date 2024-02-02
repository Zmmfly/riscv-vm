#ifndef __ZMMFLY_RV32_A_H__
#define __ZMMFLY_RV32_A_H__

#include "zmmfly/rv32_base.h"

namespace zmmfly::rv32::v1
{

class A
: public inst_intf_t
{

public:
    A() {};

    std::string name();

    rv_err_t execute_normal(uint32_t inst, registers_t& regs, bus_t& bus, inst_map_t& inst_map);
};

};

#endif /* __ZMMFLY_RV32_A_H__ */
