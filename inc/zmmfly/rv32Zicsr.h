#ifndef __ZMMFLY_RV32_ZICSR_H__
#define __ZMMFLY_RV32_ZICSR_H__

#include "zmmfly/rv32_base.h"

namespace zmmfly::rv32
{

class Zicsr
: public inst_intf_t
{

public:
    Zicsr() {};

    rv_err_t execute_normal(uint32_t inst, registers_t& regs, bus_t& bus);
};

};

#endif /* __ZMMFLY_RV32_ZICSR_H__ */
