#include "rvvm/rv32_registers.h"
#include <string.h>

namespace rvvm
{

rv32_registers::rv32_registers()
{
    memset(x, 0, sizeof(x));
    pc    = 0;
    cycle = 0;
}

}
