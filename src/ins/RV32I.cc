#include "ins/RV32I.h"

using namespace rvvm;

#define idec(name) void RV32I::i_##name(rv32_registers &regs, rv32_periph_collect &periphs, rv32_ins_t ins, uint32_t &used_cycle)

RV32I::RV32I()
{}

RV32I::~RV32I()
{}

bool RV32I::had_instruction(rv32_ins_t ins)
{
    switch(ins.opcode)
    {
        case 0b0110111: // LUI
        case 0b0010111: // AUIPC
        case 0b1101111: // JAL
        case 0b0100011: // SB & SH & SW
            return true;

        case 0b1100111: // JALR
            return (ins.I.funct3 == 0b000);
        case 0b1100011:
            switch(ins.B.funct3)
            {
                case 0b000: // BEQ
                case 0b001: // BNE
                case 0b100: // BLT
                case 0b101: // BGE
                case 0b110: // BLTU
                case 0b111: // BGEU
                    return true;
                default:
                    return false;
            }
        case 0b0000011:
            switch (ins.I.funct3)
            {
                case 0b000: // LB
                case 0b001: // LH
                case 0b010: // LW
                case 0b100: // LBU
                case 0b101: // LHU
                    return true;
                default:
                    return false;
            }
        case 0b0010011:
            switch(ins.I.funct3)
            {
                case 0b000: // ADDI
                case 0b010: // SLTI
                case 0b011: // SLTIU
                case 0b100: // XORI
                case 0b110: // ORI
                case 0b111: // ANDI
                    return true;
                case 0b001: // SLLI
                    return (ins.I.funct7 == 0b0000000);
                case 0b101: // SRLI & SRAI
                    return (ins.I.funct7 == 0b0000000 || ins.I.funct7 == 0b0100000);
                default:
                    return false;
            }
        case 0b0110011:
            switch(ins.I.funct3)
            {
                case 0b000: // ADD & SUB
                case 0b101: // SRL & SRA
                    if (ins.I.funct7 != 0b0000000 || ins.I.funct7 != 0b0100000) return false;
                case 0b001: // SLL
                case 0b010: // SLT
                case 0b011: // SLTU
                case 0b100: // XOR
                case 0b110: // OR
                case 0b111: // AND
                    return ins.R.funct7 == 0b0000000;
            }
        case 0b0001111:
            return (ins.I.funct3 == 0b000 || ins.I.funct3 == 0b001);
        case 0b1110011:
            switch(ins.I.funct3)
            {
                case 0b000: // ECALL & EBREAK
                    return (ins.I.funct7 == 0b0000000 || ins.I.funct7 == 0b0100000);
                case 0b001: // CSRRW
                case 0b010: // CSRRS
                case 0b011: // CSRRC
                case 0b101: // CSRRWI
                case 0b110: // CSSRRSI
                case 0b111: // CSRRCI
                    return true;
                default:
                    return false;
            }
        default:
            return false;
    }
}

std::string RV32I::name()
{
    return "RV32I";
}

bool RV32I::execute(rv32_registers &regs, rv32_periph_collect &periphs, rv32_ins_t ins, uint32_t &used_cycle)
{
    switch (ins.opcode)
    {
    }
}

idec(lui)
{
    uint32_t value = ins.U.imm_31_12;
    value <<= 12;
    regs.write(ins.U.rd, value);
    used_cycle  = 1;
}

idec(auipc)
{
    uint32_t value = ins.U.imm_31_12;
    value <<= 12;
    value += regs.pc;
    regs.write(ins.U.rd, value);
    regs.pc    += 4;
    used_cycle  = 1;
}

idec(jal)
{
    regs.write(ins.J.rd, regs.pc + 4);
    regs.pc += rv32_sext(ins.J.offset, 12);
    used_cycle  = 1;
}

idec(jalr)
{
    uint32_t t = regs.pc + 4;
    regs.pc = (regs.read(ins.I.rs1) + rv32_sext(ins.I.offset, 12)) & 0xFFFFFFFE;
    regs.write(ins.I.rd, t);
    used_cycle  = 1;
}

idec(beq)
{
    uint32_t offset = 0;
    offset |= ins.B.imm_12;
    offset <<= 1;
    offset |= ins.B.imm_11;
    offset <<= 10;
    offset |= ins.B.imm_10_5;
    offset <<= 4;
    offset |= ins.B.imm_4_1;
    offset <<= 1;
    if (regs.read(ins.B.rs1) == regs.read(ins.B.rs2))
    {
        regs.pc += rv32_sext(offset, 13);
        used_cycle  = 1;
    }
    else
    {
        used_cycle  = 2;
    }
}

idec(bne)
{
    uint32_t offset = 0;
    offset |= ins.B.imm_12;
    offset <<= 1;
    offset |= ins.B.imm_11;
    offset <<= 10;
    offset |= ins.B.imm_10_5;
    offset <<= 4;
    offset |= ins.B.imm_4_1;
    offset <<= 1;
    if (regs.read(ins.B.rs1) != regs.read(ins.B.rs2))
    {
        regs.pc += rv32_sext(offset, 13);
        used_cycle  = 1;
    }
    else
    {
        used_cycle  = 2;
    }
}

idec(blt)
{
    uint32_t offset = 0;
    offset |= ins.B.imm_12;
    offset <<= 1;
    offset |= ins.B.imm_11;
    offset <<= 10;
    offset |= ins.B.imm_10_5;
    offset <<= 4;
    offset |= ins.B.imm_4_1;
    offset <<= 1;
    if ((int32_t)regs.read(ins.B.rs1) < (int32_t)regs.read(ins.B.rs2))
    {
        regs.pc += rv32_sext(offset, 13);
        used_cycle  = 1;
    }
    else
    {
        used_cycle  = 2;
    }
}

idec(bge)
{
    uint32_t offset = 0;
    offset |= ins.B.imm_12;
    offset <<= 1;
    offset |= ins.B.imm_11;
    offset <<= 10;
    offset |= ins.B.imm_10_5;
    offset <<= 4;
    offset |= ins.B.imm_4_1;
    offset <<= 1;
    if ((int32_t)regs.read(ins.B.rs1) >= (int32_t)regs.read(ins.B.rs2))
    {
        regs.pc += rv32_sext(offset, 13);
        used_cycle  = 1;
    }
    else
    {
        used_cycle  = 2;
    }
}

idec(bltu)
{
    uint32_t offset = 0;
    offset |= ins.B.imm_12;
    offset <<= 1;
    offset |= ins.B.imm_11;
    offset <<= 10;
    offset |= ins.B.imm_10_5;
    offset <<= 4;
    offset |= ins.B.imm_4_1;
    offset <<= 1;
    if (regs.read(ins.B.rs1) < regs.read(ins.B.rs2))
    {
        regs.pc += rv32_sext(offset, 13);
        used_cycle  = 1;
    }
    else
    {
        used_cycle  = 2;
    }
}

idec(bgeu)
{
    uint32_t offset = 0;
    offset |= ins.B.imm_12;
    offset <<= 1;
    offset |= ins.B.imm_11;
    offset <<= 10;
    offset |= ins.B.imm_10_5;
    offset <<= 4;
    offset |= ins.B.imm_4_1;
    offset <<= 1;
    if (regs.read(ins.B.rs1) >= regs.read(ins.B.rs2))
    {
        regs.pc += rv32_sext(offset, 13);
        used_cycle  = 1;
    }
    else
    {
        regs.pc += 4;
        used_cycle  = 2;
    }
}

idec(lb)
{
    uint32_t cycle = 0;
    uint32_t addr = regs.read(ins.I.rs1) + rv32_sext(ins.I.offset, 12);
    uint8_t data = periphs.rd_u8(addr, cycle);
    regs.write(ins.I.rd, rv32_sext(data, 8));
    used_cycle  = cycle + 1;
    regs.pc += 4;
}

idec(lh)
{
    uint32_t cycle = 0;
    uint32_t addr = regs.read(ins.I.rs1) + rv32_sext(ins.I.offset, 12);
    uint16_t data = periphs.rd_u16(addr, cycle);
    regs.write(ins.I.rd, rv32_sext(data, 16));
    used_cycle  = cycle + 1;
    regs.pc += 4;
}

idec(lw)
{
    uint32_t cycle = 0;
    uint32_t addr = regs.read(ins.I.rs1) + rv32_sext(ins.I.offset, 12);
    uint32_t data = periphs.rd_u32(addr, cycle);
    regs.write(ins.I.rd, data);
    used_cycle  = cycle + 1;
    regs.pc += 4;
}

idec(lbu)
{
    uint32_t cycle = 0;
    uint32_t addr = regs.read(ins.I.rs1) + rv32_sext(ins.I.offset, 12);
    uint8_t data = periphs.rd_u8(addr, cycle);
    regs.write(ins.I.rd, data);
    used_cycle  = cycle + 1;
    regs.pc += 4;
}

idec(lhu)
{
    uint32_t cycle = 0;
    uint32_t addr = regs.read(ins.I.rs1) + rv32_sext(ins.I.offset, 12);
    uint16_t data = periphs.rd_u16(addr, cycle);
    regs.write(ins.I.rd, data);
    used_cycle  = cycle + 1;
    regs.pc += 4;
}

idec(sb)
{
    uint32_t offset = 0;
    uint32_t cycle = 0;
    offset = ins.S.offset_11_5;
    offset <<= 5;
    offset |= ins.S.offset_4_0;
    uint32_t addr = regs.read(ins.S.rs1) + rv32_sext(offset, 12);
    periphs.wr_u8(addr, regs.read(ins.S.rs2), cycle);
    used_cycle  = cycle + 1;
    regs.pc += 4;
}

idec(sh)
{
    uint32_t offset = 0;
    uint32_t cycle = 0;
    offset = ins.S.offset_11_5;
    offset <<= 5;
    offset |= ins.S.offset_4_0;
    uint32_t addr = regs.read(ins.S.rs1) + rv32_sext(offset, 12);
    periphs.wr_u16(addr, regs.read(ins.S.rs2), cycle);
    used_cycle  = cycle + 1;
    regs.pc += 4;
}

idec(sw)
{
    uint32_t offset = 0;
    uint32_t cycle = 0;
    offset = ins.S.offset_11_5;
    offset <<= 5;
    offset |= ins.S.offset_4_0;
    uint32_t addr = regs.read(ins.S.rs1) + rv32_sext(offset, 12);
    periphs.wr_u32(addr, regs.read(ins.S.rs2), cycle);
    used_cycle  = cycle + 1;
    regs.pc += 4;
}

idec(addi)
{
    regs.write(ins.I.rd, regs.read(ins.I.rs1) + rv32_sext(ins.I.imm, 12));
    regs.pc += 4;
    used_cycle  = 1;
}

idec(slti)
{
    if (regs.read(ins.I.rs1) < rv32_sext(ins.I.imm, 12))
    {
        regs.write(ins.I.rd, 1);
    }
    else
    {
        regs.write(ins.I.rd, 0);
    }
    regs.pc += 4;
    used_cycle  = 1;
}

idec(sltiu)
{
    if (regs.read(ins.I.rs1) < (uint32_t)rv32_sext(ins.I.imm, 12))
    {
        regs.write(ins.I.rd, 1);
    }
    else
    {
        regs.write(ins.I.rd, 0);
    }
    regs.pc += 4;
    used_cycle  = 1;
}

idec(xori)
{
    regs.write(ins.I.rd, regs.read(ins.I.rs1) ^ rv32_sext(ins.I.imm, 12));
    regs.pc += 4;
    used_cycle  = 1;
}

idec(ori)
{
    regs.write(ins.I.rd, regs.read(ins.I.rs1) | rv32_sext(ins.I.imm, 12));
    regs.pc += 4;
    used_cycle  = 1;
}

idec(andi)
{
    regs.write(ins.I.rd, regs.read(ins.I.rs1) & rv32_sext(ins.I.imm, 12));
    regs.pc += 4;
    used_cycle  = 1;
}

idec(slli)
{
    regs.write(ins.I.rd, regs.read(ins.I.rs1) << ins.I.shamt);
    regs.pc += 4;
    used_cycle  = 1;
}

idec(srli)
{
    regs.write(ins.I.rd, regs.read(ins.I.rs1) >> ins.I.shamt);
    regs.pc += 4;
    used_cycle  = 1;
}

idec(srai)
{
    if (ins.I.shamt & 0x20 == 0) return;
    regs.write(ins.I.rd, (int32_t)regs.read(ins.I.rs1) >> ins.I.shamt);
    regs.pc += 4;
    used_cycle  = 1;
}

idec(add)
{
    regs.write(ins.R.rd, regs.read(ins.R.rs1) + regs.read(ins.R.rs2));
    regs.pc += 4;
    used_cycle  = 1;
}

idec(sub)
{
    regs.write(ins.R.rd, regs.read(ins.R.rs1) - regs.read(ins.R.rs2));
    regs.pc += 4;
    used_cycle  = 1;
}

idec(sll)
{
    regs.write(ins.R.rd, regs.read(ins.R.rs1) << (regs.read(ins.R.rs2) & 0x1f));
    regs.pc += 4;
    used_cycle  = 1;
}

idec(slt)
{
    if (regs.read(ins.R.rs1) < (int32_t)regs.read(ins.R.rs2))
    {
        regs.write(ins.R.rd, 1);
    }
    else
    {
        regs.write(ins.R.rd, 0);
    }
    regs.pc += 4;
    used_cycle  = 1;
}

idec(sltu)
{
    if (regs.read(ins.R.rs1) < regs.read(ins.R.rs2))
    {
        regs.write(ins.R.rd, 1);
    }
    else
    {
        regs.write(ins.R.rd, 0);
    }
    regs.pc += 4;
    used_cycle  = 1;
}

idec(xor)
{
    regs.write(ins.R.rd, regs.read(ins.R.rs1) ^ regs.read(ins.R.rs2));
    regs.pc += 4;
    used_cycle  = 1;
}

idec(srl)
{
    regs.write(ins.R.rd, regs.read(ins.R.rs1) >> (regs.read(ins.R.rs2) & 0x1f));
    regs.pc += 4;
    used_cycle  = 1;
}

idec(sra)
{
    regs.write(ins.R.rd, (int32_t)regs.read(ins.R.rs1) >> (regs.read(ins.R.rs2) & 0x1f));
    regs.pc += 4;
    used_cycle  = 1;
}

idec(or)
{
    regs.write(ins.R.rd, regs.read(ins.R.rs1) | regs.read(ins.R.rs2));
    regs.pc += 4;
    used_cycle  = 1;
}

idec(and)
{
    regs.write(ins.R.rd, regs.read(ins.R.rs1) & regs.read(ins.R.rs2));
    regs.pc += 4;
    used_cycle  = 1;
}

idec(fence)
{
    //TODO : fence
}

idec(fence_i)
{
    //TODO : fence_i
}

idec(ecall)
{
    
}

idec(ebreak)
{}

idec(csrrw)
{}

idec(csrrs)
{}

idec(csrrc)
{}

idec(csrrwi)
{}

idec(csrrsi)
{}

idec(csrrci)
{}

#undef idec