#ifndef __ZMMFLY_RV32_D_H__
#define __ZMMFLY_RV32_D_H__

#include "zmmfly/rv32_base.h"

namespace zmmfly::rv32
{

class D
: public inst_intf_t
{

public:
    D() {};

    rv_err_t execute_normal(uint32_t inst, registers_t& regs, bus_t& bus);
};

};

#endif /* __ZMMFLY_RV32_D_H__ */
