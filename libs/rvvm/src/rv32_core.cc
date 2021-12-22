#include "rvvm/rv32_core.h"
#include <stdio.h>
namespace rvvm
{

rv32_core::rv32_core(rv32_instruction_collect &ins_collect, rv32_csr_collect &ins_csrs, rv32_registers &in_regs, rv32_periph_collect &in_periphs)
    : instructions(ins_collect), regs(in_regs), periphs(in_periphs), csrs(ins_csrs)
{
}

rv32_core::~rv32_core()
{
}

void rv32_core::set_pc_start(uint32_t pc)
{
    pc_start = pc;
}

bool rv32_core::start()
{
    m_running      = false;
    m_require_stop = false;
    m_thread       = new std::thread(&rv32_core::run, this);
    return true;
}

bool rv32_core::stop()
{
    if (m_thread)
    {
        m_require_stop = true;
        m_thread->join();
        delete m_thread;
        m_thread       = nullptr;
        m_require_stop = false;
        m_running      = false;
    }
    return true;
}

void rv32_core::run()
{
    uint32_t used_cycle = 0;
    rv32_ins_t ins;

    m_running = true;
    regs.pc = pc_start;
    while (!m_require_stop)
    {
        if ( !periphs.rd_u32(regs.pc, ins.value, used_cycle) )
        {
            m_running = false;
            break;
        }

        if ( !instructions.execute(*this, ins, used_cycle) )
        {
            m_running = false;
            printf("rv32_core::run() failed to execute instruction.\n");
            break;
        }
    }
    m_running = false;
}

};
