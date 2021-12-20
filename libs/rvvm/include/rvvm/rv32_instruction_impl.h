#ifndef __RVVM_RV32_INSTRUCTION_H__
#define __RVVM_RV32_INSTRUCTION_H__

#include <string>
#include <vector>
#include "rvvm/rv32_registers.h"
#include "rvvm/rv32_periph_impl.h"

namespace rvvm
{
class rv32_instruction_impl
{
    public:
        virtual std::string name() = 0;
        virtual bool execute(rv32_periph_collect &periphs, uint32_t instruction, uint32_t &used_cycle)     = 0;
};
}

#endif