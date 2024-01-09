#ifndef __ZMMFLY_RV32_ZIFENCE_H__
#define __ZMMFLY_RV32_ZIFENCE_H__

#include "zmmfly/rv32_base.h"

namespace zmmfly::rv32
{

class Zifence
: public inst_intf_t
{

public:
    Zifence() {};

    rv_err_t execute_normal(uint32_t inst, registers_t& regs, bus_t& bus);
};

};

#endif /* __ZMMFLY_RV32_ZIFENCE_H__ */
