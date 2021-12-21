#include "rvvm/rv32_core.h"
#include <stdio.h>
namespace rvvm
{

rv32_core::rv32_core(rv32_instruction_collect &instruction_collect, rv32_registers &regs, rv32_periph_collect &periphs)
    : m_instruction_collect(instruction_collect), m_regs(regs), m_periphs(periphs)
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
    m_regs.pc = pc_start;
    while (!m_require_stop)
    {
        if ( !m_periphs.read(m_regs.pc, ins.value, used_cycle) )
        {
            m_running = false;
            break;
        }

        if ( !m_instruction_collect.execute(m_regs, m_periphs, ins, used_cycle) )
        {
            m_running = false;
            printf("rv32_core::run() failed to execute instruction.\n");
            break;
        }
    }
    m_running = false;
}

};