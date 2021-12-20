#ifndef __RVVM_RV32_PERIPH_IMPL_H__
#define __RVVM_RV32_PERIPH_IMPL_H__

#include <cstddef>
#include <stdint.h>
#include <vector>

namespace rvvm
{
class rv32_periph_impl
{
    public:
        virtual bool had_addr(uint32_t addr)                                  = 0;
        virtual bool read(uint32_t addr, uint32_t &out, uint32_t &used_cycle) = 0;
        virtual bool write(uint32_t addr, uint32_t in, uint32_t &used_cycle)  = 0;
};

class rv32_periph_collect
{
    public:
        rv32_periph_collect();
        ~rv32_periph_collect();

        void add(rv32_periph_impl *periph);
        bool had_addr(uint32_t addr);
        bool read(uint32_t addr, uint32_t &out, uint32_t &used_cycle);
        bool write(uint32_t addr, uint32_t in, uint32_t &used_cycle);
    
    private:
        std::vector<rv32_periph_impl *> m_collect;
};

// typedef std::vector<rv32_periph_impl &> periphs_t;

}

#endif