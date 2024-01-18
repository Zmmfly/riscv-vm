#include "zmmfly/rv32I.h"

namespace zmmfly::rv32::v1
{

I::I()
{}

std::string I::name()
{
    return "I";
}

rv_err_t I::execute_normal(uint32_t inst, registers_t& regs, bus_t& bus, inst_map_t& inst_map)
{
    rv_err_t res = RV_EUNSUPPORTED;
    inst_type& iref = reinterpret_cast<inst_type&>(inst);
    switch(iref.opcode) {
        case 0b00'000'11:   /* LOAD */ {
            switch (iref.I.func3) {
                case 0b000: /* I LB, Load Byte */  {
                    // x[rd] = sext( M[ x[rs1] + sext(offset) ][7:0] )
                    uint32_t addr = regs.x[iref.I.rs1] + rv::sext<int32_t>(iref.I.imm, 12);
                    uint8_t v = 0;
                    res = bus.read(addr, &v, 1);
                    regs.x[iref.I.rd] = rv::sext<int32_t>(v, 8);
                    break;
                }
                case 0b001: /* I LH, Load Half */ {
                    // x[rd] = sext( M[ x[rs1] + sext(offset) ][15:0] )
                    uint32_t addr = regs.x[iref.I.rs1] + rv::sext<int32_t>(iref.I.imm, 12);
                    uint16_t v = 0;
                    res = bus.read(addr, &v, 2);
                    regs.x[iref.I.rd] = rv::sext<int32_t>(v, 16);
                    break;
                }
                case 0b010: /* I LW, Load Word */ {
                    // x[rd] = sext( M[ x[rs1] + sext(offset) ][31:0] )
                    uint32_t addr = regs.x[iref.I.rs1] + rv::sext<int32_t>(iref.I.imm, 12);
                    uint32_t v = 0;
                    res = bus.read(addr, &v, 4);
                    regs.x[iref.I.rd] = rv::sext<int32_t>(v, 32);
                    break;
                }
                case 0b100: /* I LBU, Load Byte(U) */ {
                    // x[rd] = M[ x[rs1] + sext(offset) ][7:0]
                    uint32_t addr = regs.x[iref.I.rs1] + rv::sext<int32_t>(iref.I.imm, 12);
                    uint8_t v = 0;
                    res = bus.read(addr, &v, 1);
                    regs.x[iref.I.rd] = v;
                    break;
                }
                case 0b101: /* I LHU, Load Half(U) */ {
                    // x[rd] = M[ x[rs1] + sext(offset) ][15:0]
                    uint32_t addr = regs.x[iref.I.rs1] + rv::sext<int32_t>(iref.I.imm, 12);
                    uint16_t v = 0;
                    res = bus.read(addr, &v, 2);
                    regs.x[iref.I.rd] = v;
                    break;
                }
            }
            break;
        }
        case 0b00'011'11:   /* MISC-MEM */ {
            if (iref.fence.func3 == 0b000) return RV_EOK;
            /* FENCE */
            if (m_fence)m_fence();
        }
        case 0b00'100'11:   /* OP-IMM */ {
            switch (iref.I.func3) {
                case 0b000: { /* I ADDI */
                    // x[rd] = x[rs1] + sext(imm)
                    regs.x[iref.I.rd] = regs.x[iref.I.rs1] + rv::sext<int32_t>(iref.I.imm, 12);
                    res = RV_EOK;
                    break;
                }
                case 0b010: { /* I SLTI */
                    // x[rd] = (signed) x[rs1] < sext(imm) ? 1 : 0
                    regs.x[iref.I.rd] = (int32_t)regs.x[iref.I.rs1] < rv::sext<int32_t>(iref.I.imm, 12) ? 1 : 0;
                    res = RV_EOK;
                    break;
                }
                case 0b011: { /* I SLTIU */
                    // x[rd] = (unsigned) x[rs1] < (unsigned) sext(imm) ? 1 : 0
                    regs.x[iref.I.rd] = regs.x[iref.I.rs1] < (uint32_t)rv::sext<int32_t>(iref.I.imm, 12);
                    res = RV_EOK;
                    break;
                }
                case 0b100: { /* I XORI */
                    // x[rd] = x[rs1] ^ sext[imm]
                    regs.x[iref.I.rd] = regs.x[iref.I.rs1] ^ rv::sext<int32_t>(iref.I.imm, 12);
                    res = RV_EOK;
                    break;
                }
                case 0b110: { /* I ORI */
                    // x[rd] = x[rs1] | sext[imm]
                    regs.x[iref.I.rd] = regs.x[iref.I.rs1] | rv::sext<int32_t>(iref.I.imm, 12);
                    res = RV_EOK;
                    break;
                }
                case 0b111: { /* I ANDI */
                    // x[rd] = x[rs1] & sext[imm]
                    regs.x[iref.I.rd] = regs.x[iref.I.rs1] & rv::sext<int32_t>(iref.I.imm, 12);
                    res = RV_EOK;
                    break;
                }
                default: break;
            }
        }
        case 0b00'101'11: { /* U AUIPC */
            // x[rd] = pc + sext(imm << 12) imm为移位前的值
            regs.x[iref.U.rd] = regs.pc + rv::sext<int32_t>((uint32_t)iref.U.imm31_12 << 12, 32);
            res = RV_EOK;
            break;
        }
        case 0b01'000'11: { /* STORE */
            switch(iref.S.func3) {
                case 0b000: { /* S SB, Store Byte */
                    // M[ x[rs1] + sext(imm) ] = x[rs2][7:0]
                    uint32_t addr = regs.x[iref.S.rs1] + rv::sext<int32_t>((uint32_t)iref.S.imm11_5 << 5|iref.S.imm4_0, 12);
                    uint8_t  val  = regs.x[iref.S.rs2];
                    res = bus.write(addr, &val, 1);
                    break;
                }
                case 0b001: { /* S SH, Store Half */
                    // M[ x[rs1] + sext(imm) ] = x[rs2][15:0]
                    uint32_t addr = regs.x[iref.S.rs1] + rv::sext<int32_t>((uint32_t)iref.S.imm11_5 << 5|iref.S.imm4_0, 12);
                    uint16_t  val  = regs.x[iref.S.rs2];
                    res = bus.write(addr, &val, 2);
                    break;
                }
                case 0b010: { /* S SW, Store Word */
                    // M[ x[rs1] + sext(imm) ] = x[rs2][31:0]
                    uint32_t addr = regs.x[iref.S.rs1] + rv::sext<int32_t>((uint32_t)iref.S.imm11_5 << 5|iref.S.imm4_0, 12);
                    uint32_t  val  = regs.x[iref.S.rs2];
                    res = bus.write(addr, &val, 4);
                    break;
                }
            }
        }
        case 0b01'100'11: { /* OP */ 
            switch(iref.R.func3) {
                case 0b000: { /* R ADD and SUB */
                    if (iref.R.func7 == 0b000'0000) {           /* R ADD */
                        // x[rd] = x[rs1] + x[rs2]
                        regs.x[iref.R.rs1] = regs.x[iref.R.rs1] + regs.x[iref.R.rs2];
                        res = RV_EOK;
                    } else if (iref.R.func7 == 0b010'0000) {    /* R SUB */
                        // x[rd] = x[rs1] - x[rs2]
                        regs.x[iref.R.rs1] = regs.x[iref.R.rs1] - regs.x[iref.R.rs2];
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
                    regs.x[iref.R.rd] = (int32_t)regs.x[iref.R.rs1] < (int32_t)regs.x[iref.R.rs2] ? 1 : 0;
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
                        regs.x[iref.R.rd] = (int32_t)regs.x[iref.R.rs1] >> (regs.x[iref.R.rs2] & 0x1f);
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
        }
        case 0b01'101'11: { /* U LUI */
            // x[rd] = sext( (uint32_t)imm_31_12 << 12 )
            regs.x[iref.U.rd] = rv::sext<int32_t>((uint32_t)iref.U.imm31_12 << 12, 32);
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
                    v.imm12   = iref.B.immb12;

                    if (regs.x[iref.B.rs1] == regs.x[iref.B.rs2]) regs.pc += rv::sext<int32_t>(v.imm, 13);
                    res = RV_EOK;
                    break;
                }
                case 0b001: { /* B BNE */
                    // if ( x[rs1] != x[rs2] ) pc += sext(offset)
                    instB_imm v = {0}; 

                    /* assign imm */
                    v.imm4_1  = iref.B.imm4_1;
                    v.imm10_5 = iref.B.imm10_5;
                    v.imm12   = iref.B.immb12;

                    if (regs.x[iref.B.rs1] != regs.x[iref.B.rs2]) regs.pc += rv::sext<int32_t>(v.imm, 13);
                    res = RV_EOK;
                    break;
                }
                case 0b100: { /* B BLT */
                    // if ( (signed) x[rs1] < (signed) x[rs2] ) pc += sext(offset)
                    instB_imm v = {0}; 

                    /* assign imm */
                    v.imm4_1  = iref.B.imm4_1;
                    v.imm10_5 = iref.B.imm10_5;
                    v.imm12   = iref.B.immb12;

                    if ((int32_t)regs.x[iref.B.rs1] < (int32_t)regs.x[iref.B.rs2]) regs.pc += rv::sext<int32_t>(v.imm, 13);
                    res = RV_EOK;
                    break;
                }
                case 0b101: { /* B BGE */
                    // if ( (signed) x[rs1] >= (signed) x[rs2] ) pc += sext(offset)
                    instB_imm v = {0}; 

                    /* assign imm */
                    v.imm4_1  = iref.B.imm4_1;
                    v.imm10_5 = iref.B.imm10_5;
                    v.imm12   = iref.B.immb12;

                    if ((int32_t)regs.x[iref.B.rs1] >= (int32_t)regs.x[iref.B.rs2]) regs.pc += rv::sext<int32_t>(v.imm, 13);
                    res = RV_EOK;
                    break;
                }
                case 0b110: { /* B BLTU */
                    // if ( x[rs1] < x[rs2] ) pc += sext(offset)
                    instB_imm v = {0}; 

                    /* assign imm */
                    v.imm4_1  = iref.B.imm4_1;
                    v.imm10_5 = iref.B.imm10_5;
                    v.imm12   = iref.B.immb12;

                    if (regs.x[iref.B.rs1] < regs.x[iref.B.rs2]) regs.pc += rv::sext<int32_t>(v.imm, 13);
                    res = RV_EOK;
                }
                case 0b111: { /* B BGEU */
                    // if ( x[rs1] >= x[rs2] ) pc += sext(offset)
                    instB_imm v = {0}; 

                    /* assign imm */
                    v.imm4_1  = iref.B.imm4_1;
                    v.imm10_5 = iref.B.imm10_5;
                    v.imm12   = iref.B.immb12;

                    if (regs.x[iref.B.rs1] >= regs.x[iref.B.rs2]) regs.pc += rv::sext<int32_t>(v.imm, 13);
                    res = RV_EOK;
                    break;
                }
                default: break;
            }
        }
        case 0b11'001'11: { /* I JALR */
            if (iref.I.func3 != 0b000) break;
            // 1. x[rd] = pc + 4
            // 2. pc = (x[rs1] + sext(offset)) & ~1
            regs.x[iref.I.rd] = regs.pc + 4;
            regs.pc = (regs.x[iref.I.rs1] + rv::sext<int32_t>(iref.I.imm, 12)) & (~(uint32_t)1);
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
            regs.pc += rv::sext<int32_t>(v.imm, 21);
        }
        case 0b11'100'11: { /* SYSTEM */
            if (iref.I.func3 != 0b000) break;
            if (iref.I.imm == 0) {              /* I ECALL */
                if (m_ecall) m_ecall();
            } else if (iref.I.imm == 1) {       /* I EBREAK */
                if (m_ebreak) m_ebreak();
            }
            break;
        }
        default: break;
    }
    return res;
}

rv_err_t I::set(std::string k, std::any v)
{
    rv_err_t res = RV_ETYPE;
    if (k == "ecall" && v.type() == typeid(std::function<void(void)>))
    {
        m_ecall = std::any_cast<std::function<void(void)>>(v);
        res = RV_EOK;
    }

    if (k == "ebreak" && v.type() == typeid(std::function<void(void)>))
    {
        m_ebreak = std::any_cast<std::function<void(void)>>(v);
        res = RV_EOK;
    }

    if (k == "fence" && v.type() == typeid(std::function<void(void)>))
    {
        m_fence = std::any_cast<std::function<void(void)>>(v);
        res = RV_EOK;
    }
    return res;
}

};
