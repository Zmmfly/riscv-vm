#include "rvvm/rv32_instruction_impl.h"

namespace rvvm
{

rv32_instruction_collect::rv32_instruction_collect()
{}

rv32_instruction_collect::~rv32_instruction_collect()
{
    for (auto &instruction : m_collect)
    {
        delete instruction;
    }
}

bool rv32_instruction_collect::had_instruction(rv32_ins_t ins)
{
    for (auto &instruction : m_collect)
    {
        if (instruction->had_instruction(ins))
        {
            return true;
        }
    }
    return false;
}

void rv32_instruction_collect::add(rv32_instruction_impl *ins)
{
    m_collect.push_back(ins);
}

bool rv32_instruction_collect::execute(rv32_registers &regs, rv32_periph_collect &periphs, rv32_ins_t ins, uint32_t &used_cycle)
{
    for (auto &instruction : m_collect)
    {
        if (instruction->had_instruction(ins))
        {
            return instruction->execute(regs, periphs, ins, used_cycle);
        }
    }
    return false;
}


};