#include "rvvm/rv32_instruction_impl.h"
#include "rvvm/rv32_core.h"

namespace rvvm
{

uint32_t rv32_sext(uint32_t in, uint8_t bits)
{
    uint32_t out = in;
    if (in & (1 << (bits - 1)))
    {
        out |= (0xFFFFFFFF << bits);
    }
    return out;
}

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
    m_error = true;
    return false;
}

bool rv32_instruction_collect::had_error()
{
    return m_error;
}

void rv32_instruction_collect::add(rv32_instruction_impl *ins)
{
    m_collect.push_back(ins);
}

bool rv32_instruction_collect::execute(rv32_core &core, rv32_ins_t ins, uint32_t &used_cycle)
{
    for (auto &instruction : m_collect)
    {
        if (instruction->had_instruction(ins))
        {
            m_error = instruction->execute(core, ins, used_cycle);
            return !m_error;
        }
    }
    return false;
}


};
