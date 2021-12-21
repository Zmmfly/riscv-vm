#ifndef __RVVM_RV32_CORE_H__
#define __RVVM_RV32_CORE_H__

#include <thread>
#include "rvvm/rv32_registers.h"
#include "rvvm/rv32_periph_impl.h"
#include "rvvm/rv32_instruction_impl.h"

namespace rvvm
{

class rv32_core
{
    public:
        rv32_core(rv32_instruction_collect &instruction_collect, rv32_registers &regs, rv32_periph_collect &periphs);
        ~rv32_core();
        void set_pc_start(uint32_t pc);
        bool start();
        bool stop();
    
    private:
        void run();

    private:
        uint32_t     pc_start       = 0;
        bool         m_running      = false;
        bool         m_require_stop = false;
        std::thread *m_thread       = nullptr;
        rv32_instruction_collect &m_instruction_collect;
        rv32_registers &m_regs;
        rv32_periph_collect &m_periphs;
};

}

#endif