#ifndef __ZMMFLY_RV32_FP_H__
#define __ZMMFLY_RV32_FP_H__

#include "zmmfly/rv32_base.h"

namespace zmmfly::rv32
{

using namespace rv;

using fp_type = enum fp_type 
{
    FP32  = 0b00,
    FP64  = 0b01,
    FP16  = 0b10,
    FP128 = 0b11,
};

template<fp_type type=FP32, typename Twidth=uint32_t>
class FP
: public rv::inst_intf<Twidth>
{

public:
    FP():{}

    std::string name()
    {
        if constexpr (type == FP32) return "F";
        else if constexpr (type == FP64) return "D";
        else if constexpr (type == FP16) return "Zfh";
        else if constexpr (type == FP128) return "Q";
        else "Unknown";
    }

    rv_err_t execute_normal(uint32_t inst, registers_t& regs, bus_intf_t& bus, inst_map_t& inst_map)
    {
        rv_err_t res = RV_EUNSUPPORTED;
        inst_type& iref = reinterpret_cast<inst_type&>(inst);
        regs.x[0] = 0;
        do {
            /* check fmt, in future: resue for other float extension, like D */
            // if (iref.R_fp.fmt != 0b00) break;

            if (iref.opcode == 0b00'001'11) {   /* I float load */
                auto addr = regs.x[iref.I.rs1] + rv::sext<Twidth>(iref.I.imm, 12);
                uint32_t v = 0;
                res = bus.read(addr, &v, 4);
                if (res != RV_EOK) break;
                regs.f[iref.I.rd].u64 = (regs.f[iref.I.rd].u64 & 0xffffffff) | v;
            }

            else if (iref.opcode == 0b01'001'11) {  /* S fsw */
                instS_imm imm;
                imm.imm11_5 = iref.S.imm11_5;
                imm.imm4_0  = iref.S.imm4_0;

                auto addr = regs.x[iref.S.rs1] + rv::sext<uint32_t>(imm.imm, 12);
                uint32_t v = regs.f[iref.S.rs2].u64 & 0xffffffff;
                res = bus.write(addr, &v, 4);
            }

            else if (iref.opcode == 0b10'000'11) {  /* R4 fmadd */
            }

            else if (iref.opcode == 0b10'001'11) {  /* R4 fmsub */
            }

            else if (iref.opcode == 0b10'010'11) {  /* R4 fnmsub */
            }

            else if (iref.opcode == 0b10'011'11) {  /* R4 fnmadd */
            }

            else if (iref.opcode == 0b10'100'11) {  /* R others */
                bool ok = true;
                switch(iref.R_fp.rs3_func5) {
                    case 0b00'000: {    /* fadd */
                        break;
                    }
                    case 0b00'001: {    /* fsub */
                        break;
                    }
                    case 0b00'010: {    /* fmul */
                        break;
                    }
                    case 0b00'011: {    /* fdiv */
                        break;
                    }
                    case 0b01'011: {    /* fsqrt */
                        break;
                    }
                    case 0b00'100: {    /* fsgnj[n]/jx */
                        /* fsgnj */
                        if (iref.R_fp.rm == 0b000) {
                        }

                        /* fsgnjn */
                        else if (iref.R_fp.rm == 0b001) {
                        }

                        /* fsgnjx */
                        else if (iref.R_fp.rm == 0b010) {
                        }

                        else {
                            ok = false;
                        }
                        break;
                    }
                    case 0b00'101: {    /* fmin/max */
                        /* fmin */
                        if (iref.R_fp.rm == 0b000) {
                        }

                        /* fmax */
                        else if (iref.R_fp.rm == 0b001) {
                        }

                        else {
                            ok = false;
                        }
                        break;
                    }
                    case 0b11'000: {    /* fcvt.w[u] */
                        /* fcvt.w */
                        if (iref.R_fp.rs2 == 0b00000) {
                        }

                        /* fcvt.wu */
                        else if (iref.R_fp.rs2 == 0b00001) {
                        }

                        else {
                            ok = false;
                        }
                        break;
                    }

                    case 0b11'100: {    
                        if (iref.R_fp.rs2 != 0b00000) {ok = false;}

                        /* fmv.x.w */
                        else if (iref.R_fp.rm == 0b000) {
                        }

                        /* fclass */
                        else if (iref.R_fp.rm == 0b001) {
                        }
                        break;
                    }

                    case 0b10'100: {    /* fp compare */
                        /* feq */
                        if (iref.R_fp.rm == 0b010) {
                        }

                        /* flt */
                        else if (iref.R_fp.rm == 0b001) {
                        }

                        /* fle */
                        else if (iref.R_fp.rm == 0b000) {
                        }

                        else {ok = false;}
                        break;
                    }
                    case 0b11'110: {
                        if (iref.R_fp.rs2 != 0b00000) {ok = false;}

                        /* fmv.w.x */
                        else if (iref.R_fp.rm == 0b000) {
                        }
                    }
                }
            }

            else {
                break;
            }
        }while(0);
        regs.x[0] = 0;
        return res;
    }
};

};

#endif /* __ZMMFLY_RV32_F_H__ */
