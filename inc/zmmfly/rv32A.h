#ifndef __ZMMFLY_RV32_A_H__
#define __ZMMFLY_RV32_A_H__

#include "zmmfly/rv32_base.h"

namespace zmmfly::rv32
{

class A
: public inst_intf_t
{

public:
    A() {};

    rv_err_t execute_normal(uint32_t inst, registers_t& regs, bus_t& bus);
};

};

#endif /* __ZMMFLY_RV32_A_H__ */
