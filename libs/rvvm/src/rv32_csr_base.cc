#include "rvvm/rv32_csr_base.h"

namespace rvvm
{

rv32_csr_base::rv32_csr_base()
{}

rv32_csr_base::~rv32_csr_base()
{}

bool rv32_csr_base::had_addr(rv32_core *core, uint16_t addr)
{
    return false;
}

bool rv32_csr_base::rd(rv32_core *core, uint16_t addr, uint32_t &out)
{
    return false;
}

bool rv32_csr_base::wr(rv32_core *core, uint16_t addr, uint32_t in)
{
    return false;
}

};