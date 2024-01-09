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
    I() {};

    std::string name();

    rv_err_t execute_normal(uint32_t inst, registers_t& regs, bus_t& bus);

    /**
     * @brief Set the ecall callback
     * @note if used, must set before running
     * 
     * @param fn 
     */
    void set_ecall(std::function<void(void)> fn);

    /**
     * @brief Set the ebreak callback
     * @note if used, must set before running
     * 
     * @param fn 
     */
    void set_ebreak(std::function<void(void)> fn);

private:
    std::function<void(void)> m_ecall, m_ebreak;
};

};

#endif /* __ZMMFLY_RV32_I_H__ */
