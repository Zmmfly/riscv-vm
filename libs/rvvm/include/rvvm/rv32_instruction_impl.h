#ifndef __RVVM_RV32_INSTRUCTION_H__
#define __RVVM_RV32_INSTRUCTION_H__

#include <string>
#include <vector>
#include "rvvm/rv32_registers.h"
#include "rvvm/rv32_periph_impl.h"
#include "rvvm/rv32_instruction_typedef.h"
namespace rvvm
{

class rv32_core;

uint32_t rv32_sext(uint32_t in, uint8_t bits);

class rv32_instruction_impl
{
    public:
        virtual bool had_instruction(rv32_ins_t ins)                                                                           = 0;
        virtual std::string name()                                                                                             = 0;
        virtual bool execute(rv32_core &core, rv32_ins_t instruction, uint32_t &used_cycle) = 0;
};

class rv32_instruction_collect
{
    public:
        rv32_instruction_collect();
        ~rv32_instruction_collect();

        bool had_instruction(rv32_ins_t ins);
        bool had_error();
        void add(rv32_instruction_impl *instruction);
        bool execute(rv32_core &core, rv32_ins_t instruction, uint32_t &used_cycle);

    private:
        bool m_error = false;
        std::vector<rv32_instruction_impl *> m_collect;
};

}

#endif
