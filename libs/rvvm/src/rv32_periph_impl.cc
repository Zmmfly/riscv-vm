#include "rvvm/rv32_periph_impl.h"
#include <stdio.h>

namespace rvvm
{

rv32_periph_collect::rv32_periph_collect()
{}

rv32_periph_collect::~rv32_periph_collect()
{
    for (auto &periph : m_collect)
    {
        delete periph;
    }
}

/**
 * @brief Add a peripheral to the collection. Auto delete when the collection is deleted.
 * 
 * @param periph 
 */
void rv32_periph_collect::add(rv32_periph_impl *periph)
{
    m_collect.push_back(periph);
}

/**
 * @brief Check if the peripheral had the address.
 * 
 * @param addr 
 * @return true 
 * @return false 
 */
bool rv32_periph_collect::had_addr(uint32_t addr)
{
    for (auto &periph : m_collect)
    {
        if (periph->had_addr(addr))
        {
            return true;
        }
    }
    m_error = true;
    return false;
}

bool rv32_periph_collect::had_error()
{
    return m_error;
}

bool rv32_periph_collect::rd_u8(uint32_t addr, uint8_t &out, uint32_t &used_cycle)
{
    for (auto &periph : m_collect)
    {
        if (periph->had_addr(addr))
        {
            m_error = !periph->rd_u8(addr, out, used_cycle);
            return !m_error;
        }
    }
    m_error = true;
    printf("rv32_periph_collect::rd_u8: addr %08x not found\n", addr);
    return false;
}

uint8_t rv32_periph_collect::rd_u8(uint32_t addr, uint32_t &used_cycle)
{
    uint8_t data = 0;
    rd_u8(addr, data, used_cycle);
    return data;
}

bool rv32_periph_collect::rd_u16(uint32_t addr, uint16_t &out, uint32_t &used_cycle)
{
    for (auto &periph : m_collect)
    {
        if (periph->had_addr(addr))
        {
            m_error = !periph->rd_u16(addr, out, used_cycle);
            return !m_error;
        }
    }
    m_error = true;
    printf("rv32_periph_collect::rd_u16: addr %08x not found\n", addr);
    return false;
}

uint16_t rv32_periph_collect::rd_u16(uint32_t addr, uint32_t &used_cycle)
{
    uint16_t data = 0;
    rd_u16(addr, data, used_cycle);
    return data;
}

bool rv32_periph_collect::rd_u32(uint32_t addr, uint32_t &out, uint32_t &used_cycle)
{
    for (auto &periph : m_collect)
    {
        if (periph->had_addr(addr))
        {
            m_error = periph->rd_u32(addr, out, used_cycle);
            return !m_error;
        }
    }
    m_error = true;
    printf("rv32_periph_collect::rd_u32: addr %08x not found\n", addr);
    return false;
}

uint32_t rv32_periph_collect::rd_u32(uint32_t addr, uint32_t &used_cycle)
{
    uint32_t data = 0;
    rd_u32(addr, data, used_cycle);
    return data;
}

bool rv32_periph_collect::wr_u8(uint32_t addr, uint8_t in, uint32_t &used_cycle)
{
    for (auto &periph : m_collect)
    {
        if (periph->had_addr(addr))
        {
            m_error = !periph->wr_u8(addr, in, used_cycle);
            return !m_error;
        }
    }
    m_error = true;
    printf("rv32_periph_collect::wr_u8: addr %08x not found\n", addr);
    return false;
}

bool rv32_periph_collect::wr_u16(uint32_t addr, uint16_t in, uint32_t &used_cycle)
{
    for (auto &periph : m_collect)
    {
        if (periph->had_addr(addr))
        {
            m_error = !periph->wr_u16(addr, in, used_cycle);
            return !m_error;
        }
    }
    m_error = true;
    printf("rv32_periph_collect::wr_u16: addr %08x not found\n", addr);
    return false;
}

bool rv32_periph_collect::wr_u32(uint32_t addr, uint32_t in, uint32_t &used_cycle)
{
    for (auto &periph : m_collect)
    {
        if (periph->had_addr(addr))
        {
            m_error = !periph->wr_u32(addr, in, used_cycle);
            return !m_error;
        }
    }
    m_error = true;
    printf("rv32_periph_collect::wr_u32: addr %08x not found\n", addr);
    return false;
}

};
