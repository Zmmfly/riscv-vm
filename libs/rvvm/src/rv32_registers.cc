#include "rvvm/rv32_registers.h"
#include <string.h>
#include <stdio.h>

namespace rvvm
{

rv32_registers::rv32_registers()
{
    memset(x, 0, sizeof(x));
    pc    = 0;
    cycle = 0;
}

rv32_registers::~rv32_registers()
{
}

uint32_t rv32_registers::read(uint8_t reg)
{
    if (reg < 32)
    {
        return x[reg];
    }
    printf("rv32_registers::read_reg: reg %d out of range\n", reg);
    return 0;
}

void rv32_registers::write(uint8_t reg, uint32_t in)
{
    if (reg < 32)
    {
        x[reg] = in;
        return;
    }
    printf("rv32_registers::write_reg: reg %d out of range\n", reg);
}


}
