#ifndef __ZMMFLY_RV32_I_H__
#define __ZMMFLY_RV32_I_H__

#include "zmmfly/rv32_base.h"
#include <functional>

namespace zmmfly::rv32::v1
{

class I
: public inst_intf_t
{

public:
    I();

    std::string name();

    rv_err_t set(std::string k, std::any v);

    rv_err_t execute_normal(uint32_t inst, registers_t& regs, bus_t& bus, inst_map_t& inst_map);

private:
    std::function<void(void)> m_ecall, m_ebreak, m_fence;
};

};

#endif /* __ZMMFLY_RV32_I_H__ */
