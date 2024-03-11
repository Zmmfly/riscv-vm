#ifndef __ZMMFLY_RVVM_I_H__
#define __ZMMFLY_RVVM_I_H__

#include "Zmmfly/rvvm/defs.hpp"
#include "Zmmfly/rvvm/intf.hpp"

namespace Zmmfly::rvvm
{

template<typename T=uint32_t>
class I
:public inst_intf<T>
{
public:
    static_assert(std::is_same<T, uint32_t>::value || std::is_same<T, uint64_t>, 
                "T must be uint32_t or uint64_t");

    using T_sign = typename std::make_signed<T>::type;

    I(){};
    std::string name() {return "I";}

    err_t on(std::string k, vcall_t fn)
    {
        if      (k == "ecall")  m_ecall  = fn;
        else if (k == "ebreak") m_ebreak = fn;
        else if (k == "fence")  m_fence  = fn;
        else return RV_EUNSUPPORTED;
        return RV_EOK;
    }

    err_t execute_normal(uint32_t inst, registers_t& regs, bus_intf_t& bus, inst_map_t& inst_map)
    {
        err_t res = RV_EUNSUPPORTED;
        inst_type& iref = reinterpret_cast<inst_type&>(inst);
        regs.x[0] = 0;
        switch(iref.opcode) {
            case 0b00'000'11:   /* LOAD */ {
                switch (iref.I.func3) {
                    case 0b000: /* I LB, Load Byte */  {
                        // x[rd] = sext( M[ x[rs1] + sext(offset) ][7:0] )
                        T addr = regs.x[iref.I.rs1] + rv::sext<T_sign>(iref.I.imm, 12);
                        uint8_t v = 0;
                        res = bus.read(addr, &v, 1);
                        regs.x[iref.I.rd] = rv::sext<T_sign>(v, 8);
                        break;
                    }
                    case 0b001: /* I LH, Load Half */ {
                        // x[rd] = sext( M[ x[rs1] + sext(offset) ][15:0] )
                        T addr = regs.x[iref.I.rs1] + rv::sext<T_sign>(iref.I.imm, 12);
                        uint16_t v = 0;
                        res = bus.read(addr, &v, 2);
                        regs.x[iref.I.rd] = rv::sext<T_sign>(v, 16);
                        break;
                    }
                    case 0b010: /* I LW, Load Word */ {
                        // x[rd] = sext( M[ x[rs1] + sext(offset) ][31:0] )
                        T addr = regs.x[iref.I.rs1] + rv::sext<T_sign>(iref.I.imm, 12);
                        uint32_t v = 0;
                        res = bus.read(addr, &v, 4);
                        regs.x[iref.I.rd] = rv::sext<T_sign>(v, 32);
                        break;
                    }
                    case 0b100: /* I LBU, Load Byte(U) */ {
                        // x[rd] = M[ x[rs1] + sext(offset) ][7:0]
                        T addr = regs.x[iref.I.rs1] + rv::sext<T_sign>(iref.I.imm, 12);
                        uint8_t v = 0;
                        res = bus.read(addr, &v, 1);
                        regs.x[iref.I.rd] = v;
                        break;
                    }
                    case 0b101: /* I LHU, Load Half(U) */ {
                        // x[rd] = M[ x[rs1] + sext(offset) ][15:0]
                        T addr = regs.x[iref.I.rs1] + rv::sext<T_sign>(iref.I.imm, 12);
                        uint16_t v = 0;
                        res = bus.read(addr, &v, 2);
                        regs.x[iref.I.rd] = v;
                        break;
                    }
                    case 0b110: /* RV64, I LWU, Load Word(U) */ {
                        // x[rd] = M[ x[rs1] + sext(offset) ][31:0]
                        if constexpr(sizeof(T) == 8) {
                            T addr = regs.x[iref.I.rs1] + rv::sext<T_sign>(iref.I.imm, 12);
                            uint32_t v = 0;
                            res = bus.read(addr, &v, 4);
                            regs.x[iref.I.rd] = v;
                        }
                        break;
                    }
                    case 0b011: /* RV64, I LD, Load double word */ {
                        // x[rd] = M[ x[rs1] + sext(offset) ][63:0]
                        if constexpr(sizeof(T) == 8) {
                            T addr = regs.x[iref.I.rs1] + rv::sext<T_sign>(iref.I.imm, 12);
                            res = bus.read(addr, &regs.x[iref.I.rd], 8);
                        }
                    }
                }
                break;
            }
            case 0b00'011'11:   /* MISC-MEM */ {
                if (iref.fence.func3 != 0b000 || iref.fence.rd != 0 || iref.fence.rs1 != 0) break;
                /* FENCE */
                if (m_fence)m_fence(iref.u32);
                res = RV_EOK;
                break;
            }
            case 0b00'100'11:   /* OP-IMM */ {
                switch (iref.I.func3) {
                    case 0b000: { /* I ADDI */
                        // x[rd] = x[rs1] + sext(imm)
                        regs.x[iref.I.rd] = regs.x[iref.I.rs1] + rv::sext<T_sign>(iref.I.imm, 12);
                        res = RV_EOK;
                        break;
                    }
                    case 0b010: { /* I SLTI */
                        // x[rd] = (signed) x[rs1] < sext(imm) ? 1 : 0
                        regs.x[iref.I.rd] = (T_sign)regs.x[iref.I.rs1] < rv::sext<T_sign>(iref.I.imm, 12) ? 1 : 0;
                        res = RV_EOK;
                        break;
                    }
                    case 0b011: { /* I SLTIU */
                        // x[rd] = (unsigned) x[rs1] < (unsigned) sext(imm) ? 1 : 0
                        regs.x[iref.I.rd] = regs.x[iref.I.rs1] < (T)rv::sext<T_sign>(iref.I.imm, 12);
                        res = RV_EOK;
                        break;
                    }
                    case 0b100: { /* I XORI */
                        // x[rd] = x[rs1] ^ sext[imm]
                        regs.x[iref.I.rd] = regs.x[iref.I.rs1] ^ rv::sext<T_sign>(iref.I.imm, 12);
                        res = RV_EOK;
                        break;
                    }
                    case 0b110: { /* I ORI */
                        // x[rd] = x[rs1] | sext[imm]
                        regs.x[iref.I.rd] = regs.x[iref.I.rs1] | rv::sext<T_sign>(iref.I.imm, 12);
                        res = RV_EOK;
                        break;
                    }
                    case 0b111: { /* I ANDI */
                        // x[rd] = x[rs1] & sext[imm]
                        regs.x[iref.I.rd] = regs.x[iref.I.rs1] & rv::sext<T_sign>(iref.I.imm, 12);
                        res = RV_EOK;
                        break;
                    }
                    case 0b001: { /* I SLLI */
                        // x[rd] = x[rs1] << shamt
                        if constexpr(sizeof(T) == 4) {
                            regs.x[iref.I.rd] = regs.x[iref.I_s5.rs1] << iref.I_s5.shamt;
                            res = RV_EOK;
                        }
                        else if constexpr(sizeof(T) == 8) {
                            regs.x[iref.I.rd] = regs.x[iref.I_s6.rs1] << iref.I_s6.shamt;
                            res = RV_EOK;
                        }
                        break;
                    }
                    case 0b101: { /* I SRLI and SRAI */
                        if constexpr(sizeof(T) == 4) {
                            if (iref.I_s5.func7 == 0b000'0000) { /* SRLI */
                                // x[rd] = x[rs1] >> shamt
                                regs.x[iref.I_s5.rd] = regs.x[iref.I_s5.rs1] >> iref.I_s5.shamt;
                                res = RV_EOK;
                            } 
                            
                            else if (iref.I_s5.func7 == 0b010'0000){ /* SRAI */
                                // x[rd] = x[rs1] 算术右移 shamt 位
                                regs.x[iref.I_s5.rd] = ((T_sign)regs.x[iref.I_s5.rs1]) >> iref.I_s5.shamt;
                                res = RV_EOK;
                            }
                        }
                        else if constexpr(sizeof(T) == 8) {
                            if (iref.I_s6.func6 == 0b000'000) {    /* SRLI */
                                regs.x[iref.I_s6.rd] = regs.x[iref.I_s6.rs1] >> iref.I_s6.shamt;
                                res = RV_EOK;
                            }

                            else if (iref.I_s6.func6 == 0b010'000) {  /* SRAI */
                                regs.x[iref.I_s6.rd] = ((T_sign)regs.x[iref.I_s6.rs1]) >> iref.I_s6.shamt;
                                res = RV_EOK;
                            }
                        }
                        break;
                    }
                    default: break;
                }
                break;
            }
            case 0b00'110'11: { /* OP-IMM 32, RV64 */
                // TODO test
                if constexpr(sizeof(T) != 8) break;
                if (iref.I.func3 == 0b001) { /* I ADDIW */
                    // x[rd] = sext( (x[rs1] + sext(imm)) [31:0] )
                    regs.x[iref.I.rd] = sext<T_sign>( (regs.x[iref.I.rs1] + sext<T_sign>(iref.I.imm, 12)) & 0xffffffff, 32);
                    res = RV_EOK;
                }

                else if (iref.I_s5.func3 == 0b001 && iref.I_s5.func7 == 0b000'0000) { /* I SLLIW */
                    // x[rd] = sext( (x[rs1] << shamt) [31:0] )
                    regs.x[iref.I_s5.rd] = sext<T_sign>( (regs.x[iref.I_s5.rs1] << iref.I_s5.shamt) & 0xffffffff, 32 );
                    res = RV_EOK;
                }

                else if (iref.I.func3 == 0b101) { /* I SRLIW or SRAIW */
                    if (iref.I_s5.func7 == 0b000'0000) {  /* SRLIW */
                        // x[rd] = sext( x[rs1][31:0] >> shamt )
                        regs.x[iref.I_s5.rd] = sext<T_sign>( (regs.x[iref.I_s5.rs1] & 0xffffffff) >> iref.I_s5.shamt, 32 );
                        res = RV_EOK;
                    }

                    else if (iref.I_s5.func7 == 0b010'0000) { /* SRAIW */
                        // x[rd] = sext( x[rs1][31:0] 算术右移 shamt )
                        regs.x[iref.I_s5.rd] = sext<T_sign>( ((int32_t)(regs.x[iref.I_s5.rs1] & 0xffffffff)) >> iref.I_s5.shamt, 32 );
                        res = RV_EOK;
                    }
                }
                break;
            }
            case 0b00'101'11: { /* U AUIPC */
                // x[rd] = pc + sext(imm << 12) imm为移位前的值
                regs.x[iref.U.rd] = regs.pc + rv::sext<T_sign>((T)iref.U.imm31_12 << 12, 32);
                res = RV_EOK;
                break;
            }
            case 0b01'000'11: { /* STORE */
                switch(iref.S.func3) {
                    case 0b000: { /* S SB, Store Byte */
                        // M[ x[rs1] + sext(imm) ] = x[rs2][7:0]
                        instS_imm imm;
                        imm.imm11_5 = iref.S.imm11_5;
                        imm.imm4_0  = iref.S.imm4_0;

                        T addr = regs.x[iref.S.rs1] + rv::sext<T_sign>( imm.imm, 12 );
                        uint8_t  val  = regs.x[iref.S.rs2];
                        res = bus.write(addr, &val, 1);
                        break;
                    }
                    case 0b001: { /* S SH, Store Half */
                        // M[ x[rs1] + sext(imm) ] = x[rs2][15:0]
                        instS_imm imm;
                        imm.imm11_5 = iref.S.imm11_5;
                        imm.imm4_0  = iref.S.imm4_0;

                        T addr = regs.x[iref.S.rs1] + rv::sext<T_sign>( imm.imm, 12 );
                        uint16_t  val = regs.x[iref.S.rs2];
                        res = bus.write(addr, &val, 2);
                        break;
                    }
                    case 0b010: { /* S SW, Store Word */
                        // M[ x[rs1] + sext(imm) ] = x[rs2][31:0]
                        instS_imm imm;
                        imm.imm11_5 = iref.S.imm11_5;
                        imm.imm4_0  = iref.S.imm4_0;

                        T addr = regs.x[iref.S.rs1] + rv::sext<T_sign>( imm.imm, 12 );
                        uint32_t  val = regs.x[iref.S.rs2];
                        res = bus.write(addr, &val, 4);
                        break;
                    }
                    case 0b011: { /* S SD, Store Double word, RV64 */
                        // M [ x[rs1] + sext(offset) ] = x[rs2][63:0]
                        if constexpr(sizeof(T) != 8) break;

                        instS_imm imm;
                        imm.imm11_5 = iref.S.imm11_5;
                        imm.imm4_0  = iref.S.imm4_0;

                        T addr = regs.x[iref.S.rs1] + rv::sext<T_sign>( imm.imm, 12 );
                        res = bus.write(addr, &regs.x[iref.S.rs2], 8);
                        break;
                    }
                }
                break;
            }
            case 0b01'100'11: { /* OP */ 
                switch(iref.R.func3) {
                    case 0b000: { /* R ADD and SUB */
                        if (iref.R.func7 == 0b000'0000) {           /* R ADD */
                            // x[rd] = x[rs1] + x[rs2]
                            regs.x[iref.R.rd] = regs.x[iref.R.rs1] + regs.x[iref.R.rs2];
                            res = RV_EOK;
                        } else if (iref.R.func7 == 0b010'0000) {    /* R SUB */
                            // x[rd] = x[rs1] - x[rs2]
                            regs.x[iref.R.rd] = regs.x[iref.R.rs1] - regs.x[iref.R.rs2];
                            res = RV_EOK;
                        }
                        break;
                    }
                    case 0b001: { /* R SLL. Shift Left Logical */
                        if (iref.R.func7 != 0b000'0000) break;
                        // x[rd] = x[rs1] << x[rs2]
                        regs.x[iref.R.rd] = regs.x[iref.R.rs1] << regs.x[iref.R.rs2];
                        res = RV_EOK;
                        break;
                    }
                    case 0b010: { /* R SLT. Shift Less Than */
                        if (iref.R.func7 != 0b000'0000) break;
                        // x[rd] = (signed) x[rs1] < (signed) x[rs2] ? 1 : 0
                        regs.x[iref.R.rd] = (T_sign)regs.x[iref.R.rs1] < (T_sign)regs.x[iref.R.rs2] ? 1 : 0;
                        res = RV_EOK;
                        break;
                    }
                    case 0b011: { /* R SLTU. Shift Less Than (U) */
                        if (iref.R.func7 != 0b000'0000) break;
                        // x[rd] = x[rs1] < x[rs2] ? 1 : 0
                        regs.x[iref.R.rd] = regs.x[iref.R.rs1] < regs.x[iref.R.rs2] ? 1 : 0;
                        res = RV_EOK;
                        break;
                    }
                    case 0b100: { /* R XOR */
                        if (iref.R.func7 != 0b000'0000) break;
                        // x[rd] = x[rs1] ^ x[rs2]
                        regs.x[iref.R.rd] = regs.x[iref.R.rs1] ^ regs.x[iref.R.rs2];
                        res = RV_EOK;
                        break;
                    }
                    case 0b101: { /* R SRL and SRA */
                        if (iref.R.func7 == 0b000'0000) {           /* R SRL */
                            // x[rd] = zext(x[rs1] >> (unsigned) x[rs2])
                            regs.x[iref.R.rd] = regs.x[iref.R.rs1] >> regs.x[iref.R.rs2];
                            res = RV_EOK;
                            break;
                        } else if (iref.R.func7 == 0b010'0000) {    /* R SRA */
                            // x[rd] = (signed) x[rs1] >> x[rs2][4:0]
                            regs.x[iref.R.rd] = (T_sign)regs.x[iref.R.rs1] >> (regs.x[iref.R.rs2] & 0x1f);
                            res = RV_EOK;
                            break;
                        }
                        break;
                    }
                    case 0b110: { /* R OR */
                        if (iref.R.func7 != 0b000'0000) break;
                        // x[rd] = x[rs1] | x[rs2]
                        regs.x[iref.R.rd] = regs.x[iref.R.rs1] | regs.x[iref.R.rs2];
                        res = RV_EOK;
                        break;
                    }
                    case 0b111: { /* R AND */
                        if (iref.R.func7 != 0b000'0000) break;
                        // x[rd] = x[rs1] & x[rs2]
                        regs.x[iref.R.rd] = regs.x[iref.R.rs1] & regs.x[iref.R.rs2];
                        res = RV_EOK;
                        break;
                    }
                    default: break;
                }
                break;
            }
            case 0b01'110'11: { /* R OP-32, RV64 */
                if constexpr(sizeof(T) != 8) break;
                // TODO test
                if (iref.R.func3 == 0b000 && iref.R.func7 == 0b000'0000) { /* ADDW */
                    // x[rd] = sext( (x[rs1] + x[rs2])[31:0] )
                    regs.x[iref.R.rd] = sext<T_sign>( (regs.x[iref.R.rs1] + regs.x[iref.R.rs2]) & 0xffffffff, 32);
                    res = RV_EOK;
                }

                else if (iref.R.func3 == 0b000 && iref.R.func7 == 0b010'0000) { /* SUBW */
                    // x[rd] = sext( (x[rs1] - x[rs2])[31:0] )
                    regs.x[iref.R.rd] = sext<T_sign>( (regs.x[iref.R.rs1] - regs.x[iref.R.rs2]) & 0xffffffff, 32);
                    res = RV_EOK;
                }

                else if (iref.R.func3 == 0b001 && iref.R.func7 == 0b000'0000) { /* SLLW */
                    // x[rd] = sext( (x[rs1] << x[rs2][4:0])[31:0] )
                    regs.x[iref.R.rd] = sext<T_sign>( ((regs.x[iref.R.rs1] << (regs.x[iref.R.rs2] & 0x1f)) & 0xffffffff), 32);
                    res = RV_EOK;
                }

                else if (iref.R.func3 == 0b101 && iref.R.func7 == 0b000'0000) { /* SRLW */
                    // x[rd] = sext( (x[rs1][31:0] >> u x[rs2][4:0]) )
                    regs.x[iref.R.rd] = sext<T_sign>( ((regs.x[iref.R.rs1]) & 0xffffffff) >> (regs.x[iref.R.rs2] & 0x1f), 32 );
                    res = RV_EOK;
                }

                else if (iref.R.func3 == 0b101 && iref.R.func7 == 0b010'0000) { /* SRAW */
                    // x[rd] = sext( (x[rs1][31:0] >> s x[rs2][4:0]) )
                    regs.x[iref.R.rd] = sext<T_sign>( (uint32_t)((regs.x[iref.R.rs1]) & 0xffffffff) >> (regs.x[iref.R.rs2] & 0x1f), 32 );
                    res = RV_EOK;
                }
                break;
            }
            case 0b01'101'11: { /* U LUI */
                // x[rd] = sext( (uint32_t)imm_31_12 << 12 )
                regs.x[iref.U.rd] = rv::sext<T_sign>(((T)iref.U.imm31_12) << 12, 32);
                res = RV_EOK;
                break;
            }
            case 0b11'000'11: { /* BRANCH */
                switch(iref.B.func3) {
                    case 0b000: { /* B BEQ */
                        // if (x[rs1] == x[rs2]) pc += sext(offset)
                        instB_imm v = {0}; 

                        /* assign imm */
                        v.imm4_1  = iref.B.imm4_1;
                        v.imm10_5 = iref.B.imm10_5;
                        v.imm11   = iref.B.immb11;
                        v.imm12   = iref.B.immb12;

                        if (regs.x[iref.B.rs1] == regs.x[iref.B.rs2]) regs.pc += rv::sext<T_sign>(v.imm, 13);
                        res = RV_EOK;
                        break;
                    }
                    case 0b001: { /* B BNE */
                        // if ( x[rs1] != x[rs2] ) pc += sext(offset)
                        instB_imm v = {0}; 

                        /* assign imm */
                        v.imm4_1  = iref.B.imm4_1;
                        v.imm10_5 = iref.B.imm10_5;
                        v.imm11   = iref.B.immb11;
                        v.imm12   = iref.B.immb12;

                        if (regs.x[iref.B.rs1] != regs.x[iref.B.rs2]) regs.pc += rv::sext<T_sign>(v.imm, 13);
                        res = RV_EOK;
                        break;
                    }
                    case 0b100: { /* B BLT */
                        // if ( (signed) x[rs1] < (signed) x[rs2] ) pc += sext(offset)
                        instB_imm v = {0}; 

                        /* assign imm */
                        v.imm4_1  = iref.B.imm4_1;
                        v.imm10_5 = iref.B.imm10_5;
                        v.imm11   = iref.B.immb11;
                        v.imm12   = iref.B.immb12;

                        if ((T_sign)regs.x[iref.B.rs1] < (T_sign)regs.x[iref.B.rs2]) regs.pc += rv::sext<T_sign>(v.imm, 13);
                        res = RV_EOK;
                        break;
                    }
                    case 0b101: { /* B BGE */
                        // if ( (signed) x[rs1] >= (signed) x[rs2] ) pc += sext(offset)
                        instB_imm v = {0}; 

                        /* assign imm */
                        v.imm4_1  = iref.B.imm4_1;
                        v.imm10_5 = iref.B.imm10_5;
                        v.imm11   = iref.B.immb11;
                        v.imm12   = iref.B.immb12;

                        if ((T_sign)regs.x[iref.B.rs1] >= (T_sign)regs.x[iref.B.rs2]) regs.pc += rv::sext<T_sign>(v.imm, 13);
                        res = RV_EOK;
                        break;
                    }
                    case 0b110: { /* B BLTU */
                        // if ( x[rs1] < x[rs2] ) pc += sext(offset)
                        instB_imm v = {0}; 

                        /* assign imm */
                        v.imm4_1  = iref.B.imm4_1;
                        v.imm10_5 = iref.B.imm10_5;
                        v.imm11   = iref.B.immb11;
                        v.imm12   = iref.B.immb12;

                        if (regs.x[iref.B.rs1] < regs.x[iref.B.rs2]) regs.pc += rv::sext<T_sign>(v.imm, 13);
                        res = RV_EOK;
                        break;
                    }
                    case 0b111: { /* B BGEU */
                        // if ( x[rs1] >= x[rs2] ) pc += sext(offset)
                        instB_imm v = {0}; 

                        /* assign imm */
                        v.imm4_1  = iref.B.imm4_1;
                        v.imm10_5 = iref.B.imm10_5;
                        v.imm11   = iref.B.immb11;
                        v.imm12   = iref.B.immb12;

                        if (regs.x[iref.B.rs1] >= regs.x[iref.B.rs2]) regs.pc += rv::sext<T_sign>(v.imm, 13);
                        res = RV_EOK;
                        break;
                    }
                    default: break;
                }
                break;
            }
            case 0b11'001'11: { /* I JALR */
                if (iref.I.func3 != 0b000) break;
                // 1. x[rd] = pc + 4
                // 2. pc = (x[rs1] + sext(offset)) & ~1
                regs.x[iref.I.rd] = regs.pc + 4;
                regs.pc = (regs.x[iref.I.rs1] + rv::sext<T_sign>(iref.I.imm, 12)) & (~(T)1);
                res = RV_EOK;
                break;
            }
            case 0b11'011'11: { /* J JAL */
                // 1. x[rd] = pc + 4
                // 2. pc += sext(offset)
                instJ_imm v = {0};

                v.imm10_1  = iref.J.imm10_1;
                v.imm11    = iref.J.immb11;
                v.imm19_12 = iref.J.imm19_12;
                v.imm20    = iref.J.immb20;

                regs.x[iref.J.rd] = regs.pc + 4;
                regs.pc += rv::sext<T_sign>(v.imm, 21);
                res = RV_EOK;
                break;
            }
            case 0b11'100'11: { /* SYSTEM */
                if (iref.I.func3 != 0b000) break;
                if (iref.I.imm == 0) {              /* I ECALL */
                    if (m_ecall) m_ecall(iref.u32);
                    res = RV_EOK;
                } else if (iref.I.imm == 1) {       /* I EBREAK */
                    if (m_ebreak) m_ebreak(iref.u32);
                    res = RV_EOK;
                }
                break;
            }
            default: break;
        }
        regs.x[0] = 0;
        return res;
    }
private:
    vcall_t m_ecall, m_ebreak, m_fence;
};

}

#endif /* __ZMMFLY_RVVM_I_H__ */
