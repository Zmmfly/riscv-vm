#include "zmmfly/rv32M.h"

namespace zmmfly::rv32::v1
{
M::M()
{}

rv_err_t M::execute_normal(uint32_t inst, registers_t& regs, bus_t& bus, inst_map_t& inst_map)
{
    rv_err_t res = RV_EUNSUPPORTED;
    inst_type& iref = reinterpret_cast<inst_type&>(inst);
    regs.x[0] = 0;
    switch(iref.opcode){
        case 0b01'100'11: {
            if (iref.func3 == 0b000) { // mul
                // x[rd] = (s32) x[rs1] * (s32) x[rs2]
            } else if (iref.func3 == 0b001) { // mulh
                // x[rd] = ((s64) x[rs1] * (s64) x[rs2]) >> XLEN
            } else if (iref.func3 == 0b010) { // mulhsu
                // x[rd] = ((s64) x[rs1] * (u64) x[rs2]) >> XLEN
            } else if (iref.func3 == 0b011) { // mulhu
                // x[rd] = ((u64) x[rs1] * (u64) x[rs2]) >> XLEN
            } else if (iref.func3 == 0b100) { // div
                // x[rd] = (s32) x[rs1] / (s32) x[rs2]
            } else if (iref.func3 == 0b101) { // divu
                // x[rd] = (u32) x[rs1] / (u32) x[rs2]
            } else if (iref.func3 == 0b110) { // rem
                // x[rd] = (s32) x[rs1] % (s32) x[rs2]
            } else if (iref.func3 == 0b111) { // remu
                // x[rd] = (u32) x[rs1] % (u32) x[rs2]
            }
            break;
        }
        default:
            break;
    }
    return res;
}


}