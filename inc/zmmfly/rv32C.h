#ifndef __ZMMFLY_RV32_C_H__
#define __ZMMFLY_RV32_C_H__

#include "zmmfly/rv32_base.h"

namespace zmmfly::rv32
{

class C
: public inst_intf_t
{

public:
    C() {};

    rv_err_t execute_normal(uint32_t inst, registers_t& regs, bus_t& bus);
};

};

#endif /* __ZMMFLY_RV32_C_H__ */
