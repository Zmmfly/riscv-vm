#include "zmmfly/rv32A.h"

namespace zmmfly::rv32::v1
{
A::A()
{}

std::string A::name()
{
    return "A";
}

rv_err_t A::execute_normal(uint32_t inst, registers_t& regs, bus_t& bus, inst_map_t& inst_map)
{
    rv_err_t res = RV_EUNSUPPORTED;
    inst_type& iref = reinterpret_cast<inst_type&>(inst);
    regs.x[0] = 0;
    switch(iref.opcode){
        case 0b01'011'11: {
            switch(iref.R_amo.func5) {
                case 0b000'10:  /* lr.w */ {
                    break;
                }
                case 0b000'11:  /* sc.w */ {
                    break;
                }
                case 0b000'01:  /* amoswap.w */ {
                    break;
                }
                case 0b000'00:  /* amoadd.w */ {
                    break;
                }
                case 0b001'00:  /* amoxor.w */ {
                    break;
                }
                case 0b011'00:  /* amoand.w */ {
                    break;
                }
                case 0b010'00:  /* amoor.w */ {
                    break;
                }
                case 0b100'00:  /* amomin.w */ {
                    break;
                }
                case 0b101'00:  /* amomax.w */ {
                    break;
                }
                case 0b110'00:  /* amominu.w */ {
                    break;
                }
                case 0b111'00:  /* amomaxu.w */ {
                    break;
                }
                default:
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
