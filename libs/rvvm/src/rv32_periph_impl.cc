#include "rvvm/rv32_periph_impl.h"

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
    return false;
}

bool rv32_periph_collect::read(uint32_t addr, uint32_t &out, uint32_t &used_cycle)
{
    for (auto &periph : m_collect)
    {
        if (periph->had_addr(addr))
        {
            return periph->read(addr, out, used_cycle);
        }
    }
    return false;
}

bool rv32_periph_collect::write(uint32_t addr, uint32_t in, uint32_t &used_cycle)
{
    for (auto &periph : m_collect)
    {
        if (periph->had_addr(addr))
        {
            return periph->write(addr, in, used_cycle);
        }
    }
    return false;
}


};
