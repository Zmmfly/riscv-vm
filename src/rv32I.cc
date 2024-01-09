#include "zmmfly/rv32I.h"

namespace zmmfly::rv32
{

I::I()
{}

rv_err_t I::execute_normal(uint32_t inst, registers_t& regs, bus_t& bus)
{
    rv_err_t res = RV_EUNSUPPORTED;
    inst_type& inst_ref = reinterpret_cast<inst_type&>(inst);
    switch(inst_ref.opcode) {
        case 0b00'000'11:   /* LOAD */ {
            switch (inst_ref.I.func3) {
                case 0b000: /* I LB, Load Byte */  {
                }
                case 0b001: /* I LH, Load Half */ {
                }
                case 0b010: /* I LW, Load Word */ {
                }
                case 0b100: /* I LBU, Load Byte(U) */ {
                }
                case 0b101: /* I LHU, Load Half(U) */ {
                }
            }
            break;
        }
        case 0b00'011'11:   /* MISC-MEM */ {
            if (inst_ref.fence.func3 == 0b000) return RV_EOK;
        }
        case 0b00'100'11:   /* OP-IMM */ {
            switch (inst_ref.I.func3) {
                case 0b000: { /* I ADDI */
                }
                case 0b010: { /* I SLTI */
                }
                case 0b011: { /* I SLTIU */
                }
                case 0b100: { /* I XORI */
                }
                case 0b110: { /* I ORI */
                }
                case 0b111: { /* I ANDI */
                }
                default: break;
            }
        }
        case 0b00'101'11: { /* U AUIPC */
        }
        case 0b01'000'11: { /* STORE */
            switch(inst_ref.S.func3) {
                case 0b000: { /* S SB, Store Byte */
                }
                case 0b001: { /* S SH, Store Half */
                }
                case 0b010: { /* S SW, Store Word */
                }
            }
        }
        case 0b01'100'11: { /* OP */ 
            switch(inst_ref.R.func3) {
                case 0b000: { /* R ADD and SUB */
                    if (inst_ref.R.func7 == 0b000'0000) {           /* R ADD */
                    } else if (inst_ref.R.func7 == 0b010'0000) {    /* R SUB */
                    }
                    break;
                }
                case 0b001: { /* R SLL. Shift Left Logical */
                    if (inst_ref.R.func7 != 0b000'0000) break;
                }
                case 0b010: { /* R SLT. Shift Less Than */
                    if (inst_ref.R.func7 != 0b000'0000) break;
                }
                case 0b011: { /* R SLTU. Shift Less Than (U) */
                    if (inst_ref.R.func7 != 0b000'0000) break;
                }
                case 0b100: { /* R XOR */
                    if (inst_ref.R.func7 != 0b000'0000) break;
                }
                case 0b101: { /* R SRL and SRA */
                    if (inst_ref.R.func7 == 0b000'0000) {           /* R SRL */
                    } else if (inst_ref.R.func7 == 0b010'0000) {    /* R SRA */
                    }
                    break;
                }
                case 0b110: { /* R OR */
                    if (inst_ref.R.func7 != 0b000'0000) break;
                }
                case 0b111: { /* R AND */
                    if (inst_ref.R.func7 != 0b000'0000) break;
                }
                default: break;
            }
        }
        case 0b01'101'11: { /* LUI */
        }
        case 0b11'000'11: { /* BRANCH */
            switch(inst_ref.B.func3) {
                case 0b000: { /* B BEQ, if (*rs1 == *rs2) PC += imm */
                }
                case 0b001: { /* B BNE, if (*rs1 != *rs2) PC += imm */
                }
                case 0b100: { /* B BLT, if (*rs1 < *rs2) PC += imm */
                }
                case 0b101: { /* B BGE, if (*rs1 >= *rs2) PC += imm */
                }
                case 0b110: { /* B BLTU */
                }
                case 0b111: { /* B BGEU */
                }
                default: break;
            }
        }
        case 0b11'001'11: { /* JALR */
            if (inst_ref.I.func3 != 0b000) break;
        }
        case 0b11'011'11: { /* JAL */
        }
        case 0b11'100'11: { /* SYSTEM */
            if (inst_ref.I.func3 != 0b000) break;
            if (inst_ref.I.imm == 0) {              /* I ECALL */
                if (m_ecall) m_ecall();
            } else if (inst_ref.I.imm == 1) {       /* I EBREAK */
                if (m_ebreak) m_ebreak();
            }
            break;
        }
        default: break;
    }
}

void I::set_ecall(std::function<void(void)> fn)
{
    m_ecall = fn;
}

void I::set_ebreak(std::function<void(void)> fn)
{
    m_ebreak = fn;
}

};
