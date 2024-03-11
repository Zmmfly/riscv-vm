#ifndef __ZMMFLY_RVVM_M_H__
#define __ZMMFLY_RVVM_M_H__

#include "Zmmfly/rvvm/defs.hpp"
#include "Zmmfly/rvvm/intf.hpp"
#include "Zmmfly/rvvm/utils.hpp"

namespace Zmmfly::rvvm
{

template<typename T=uint32_t>
class M
:public inst_intf<T>
{
public:
    static_assert(std::is_same<T, uint32_t>::value || std::is_same<T, uint64_t>, 
                "T must be uint32_t or uint64_t");

    using T_sign = typename std::make_signed<T>::type;

    M(){};
    std::string name() {return "M";}

    err_t execute_normal(uint32_t inst, registers_t& regs, bus_intf_t& bus, inst_map_t& inst_map)
    {
        err_t res = RV_EUNSUPPORTED;
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
                    T_sign lo;
                    mul_signed(reinterpret_cast<T_sign>(regs.x[iref.R.rs1]), reinterpret_cast<T_sign>(regs.x[iref.R.rs2]), lo, reinterpret_cast<T_sign&>(regs.x[iref.R.rd]));
                    res = RV_EOK;

                } else if (iref.func3 == 0b010) { // mulhsu
                    // x[rd] = ((s64) x[rs1] * (u64) x[rs2]) >> XLEN
                    // regs.x[iref.R.rd] = ( rv::sext<int64_t>(regs.x[iref.R.rs1], 32) * (uint64_t)regs.x[iref.R.rs2] ) >> 32;
                    T_sign lo;
                    mul_signed_unsigned(reinterpret_cast<T_sign>(regs.x[iref.R.rs1]), regs.x[iref.R.rs2], lo, reinterpret_cast<T_sign&>(regs.x[iref.R.rd]));
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
                        regs.x[iref.R.rd] = (T_sign)regs.x[iref.R.rs1] / (T_sign)regs.x[iref.R.rs2];
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
                        regs.x[iref.R.rd] = (T_sign)regs.x[iref.R.rs1] % (T_sign)regs.x[iref.R.rs2];
                    }
                    res = RV_EOK;

                } else if (iref.func3 == 0b111) { // remu
                    // x[rd] = (u32) x[rs1] % (u32) x[rs2]
                    if (regs.x[iref.R.rs2] == 0) { 
                        regs.x[iref.R.rd] = regs.x[iref.R.rs1];
                    } else {
                        regs.x[iref.R.rd] = regs.x[iref.R.rs1] % regs.x[iref.R.rs2];
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
};
}

#endif /* __ZMMFLY_RVVM_M_H__ */
