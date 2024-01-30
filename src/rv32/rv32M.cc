#include "zmmfly/rv32M.h"

namespace zmmfly::rv32::v1
{
M::M()
{}

std::string M::name()
{
    return "M";
}

rv_err_t M::execute_normal(uint32_t inst, registers_t& regs, bus_t& bus, inst_map_t& inst_map)
{
    rv_err_t res = RV_EUNSUPPORTED;
    inst_type& iref = reinterpret_cast<inst_type&>(inst);
    regs.x[0] = 0;
    switch(iref.opcode){
        case 0b01'100'11: {
            if (iref.func3 == 0b000) { // mul
                // x[rd] = (s32) x[rs1] * (s32) x[rs2]
                regs.x[iref.R.rd] = (int32_t)regs.x[iref.R.rs1] * (int32_t)regs.x[iref.R.rs2];
                res = RV_EOK;

            } else if (iref.func3 == 0b001) { // mulh
                // x[rd] = ((s64) x[rs1] * (s64) x[rs2]) >> XLEN
                // regs.x[iref.R.rd] = ( (int64_t)regs.x[iref.R.rs1] * (int64_t)regs.x[iref.R.rs2] ) >> 32;
                regs.x[iref.R.rd] = ( rv::sext<int64_t>(regs.x[iref.R.rs1], 32) * rv::sext<int64_t>(regs.x[iref.R.rs2], 32) ) >> 32;
                res = RV_EOK;

            } else if (iref.func3 == 0b010) { // mulhsu
                // x[rd] = ((s64) x[rs1] * (u64) x[rs2]) >> XLEN
                regs.x[iref.R.rd] = ( rv::sext<int64_t>(regs.x[iref.R.rs1], 32) * (uint64_t)regs.x[iref.R.rs2] ) >> 32;
                res = RV_EOK;

            } else if (iref.func3 == 0b011) { // mulhu
                // x[rd] = ((u64) x[rs1] * (u64) x[rs2]) >> XLEN
                regs.x[iref.R.rd] = ( (uint64_t)regs.x[iref.R.rs1] * (uint64_t)regs.x[iref.R.rs2] ) >> 32;
                res = RV_EOK;

            } else if (iref.func3 == 0b100) { // div
                // x[rd] = (s32) x[rs1] / (s32) x[rs2]
                if (regs.x[iref.R.rs2] == 0) {
                    regs.x[iref.R.rd] = 0xffffffff;
                } else {
                    regs.x[iref.R.rd] = (int32_t)regs.x[iref.R.rs1] / (int32_t)regs.x[iref.R.rs2];
                }
                res = RV_EOK;

            } else if (iref.func3 == 0b101) { // divu
                // x[rd] = (u32) x[rs1] / (u32) x[rs2]
                if (regs.x[iref.R.rs2] == 0) {
                    regs.x[iref.R.rd] = 0xffffffff;
                } else {
                    regs.x[iref.R.rd] = regs.x[iref.R.rs1] / regs.x[iref.R.rs2];
                }
                res = RV_EOK;

            } else if (iref.func3 == 0b110) { // rem
                // x[rd] = (s32) x[rs1] % (s32) x[rs2]
                if (regs.x[iref.R.rs2] == 0) { 
                    regs.x[iref.R.rd] = regs.x[iref.R.rs1];
                } else {
                    regs.x[iref.R.rd] = (int32_t)regs.x[iref.R.rs1] % (int32_t)regs.x[iref.R.rs2];
                }
                res = RV_EOK;

            } else if (iref.func3 == 0b111) { // remu
                // x[rd] = (u32) x[rs1] % (u32) x[rs2]
                if (regs.x[iref.R.rs2] == 0) { 
                    regs.x[iref.R.rd] = regs.x[iref.R.rs1];
                } else {
                    regs.x[iref.R.rd] = (uint32_t)regs.x[iref.R.rs1] % (uint32_t)regs.x[iref.R.rs2];
                }
                res = RV_EOK;

            } else {
                break;
            }
            break;
        }
        default:
            break;
    }
    regs.x[0] = 0;
    return res;
}


}