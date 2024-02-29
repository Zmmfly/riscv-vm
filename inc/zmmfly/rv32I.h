#ifndef __ZMMFLY_RV32_I_H__
#define __ZMMFLY_RV32_I_H__

#include "zmmfly/rv32_base.h"
#include <functional>

namespace zmmfly::rv32
{

class I
: public inst_intf_t
{

public:

    I();

    std::string name();

    rv_err_t on(std::string k, vcall_t fn);

    rv_err_t execute_normal(uint32_t inst, registers_t& regs, bus_t& bus, inst_map_t& inst_map);

private:
    vcall_t m_ecall, m_ebreak, m_fence;
};

};

#endif /* __ZMMFLY_RV32_I_H__ */
