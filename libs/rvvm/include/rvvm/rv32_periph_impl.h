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
        virtual bool had_addr(uint32_t addr)                                    = 0;
        virtual bool rd_u8(uint32_t addr, uint8_t &out, uint32_t &used_cycle)   = 0;
        virtual bool rd_u16(uint32_t addr, uint16_t &out, uint32_t &used_cycle) = 0;
        virtual bool rd_u32(uint32_t addr, uint32_t &out, uint32_t &used_cycle) = 0;
        virtual bool wr_u8(uint32_t addr, uint8_t in, uint32_t &used_cycle)     = 0;
        virtual bool wr_u16(uint32_t addr, uint16_t in, uint32_t &used_cycle)   = 0;
        virtual bool wr_u32(uint32_t addr, uint32_t in, uint32_t &used_cycle)   = 0;
};

class rv32_periph_collect
{
    public:
        rv32_periph_collect();
        ~rv32_periph_collect();

        void add(rv32_periph_impl *periph);
        bool had_addr(uint32_t addr);
        bool had_error();
        bool rd_u8(uint32_t addr, uint8_t &out, uint32_t &used_cycle);
        bool rd_u16(uint32_t addr, uint16_t &out, uint32_t &used_cycle);
        bool rd_u32(uint32_t addr, uint32_t &out, uint32_t &used_cycle);
        uint8_t rd_u8(uint32_t addr, uint32_t &used_cycle);
        uint16_t rd_u16(uint32_t addr, uint32_t &used_cycle);
        uint32_t rd_u32(uint32_t addr, uint32_t &used_cycle);
        bool wr_u8(uint32_t addr, uint8_t in, uint32_t &used_cycle);
        bool wr_u16(uint32_t addr, uint16_t in, uint32_t &used_cycle);
        bool wr_u32(uint32_t addr, uint32_t in, uint32_t &used_cycle);
    
    private:
        std::vector<rv32_periph_impl *> m_collect;
        bool m_error = false;
};

// typedef std::vector<rv32_periph_impl &> periphs_t;

}

#endif
