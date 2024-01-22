#include <cstdio>
#include <iostream>
#include "zmmfly/rv32I.h"
#include "tester_rv32i.h"
#include <gtest/gtest.h>
#include <fmt/core.h>

tester_rv32i tester;
zmmfly::rv32::inst_type inst;

int main(int argc, char** argv)
{
    inst.opcode = 0b0100011;
    inst.others = 0;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/*
TEST(STORE, template)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.R.func3 = ;
    inst.R.func7 = ;
    inst.R.rd    = 3;
    inst.R.rs1   = 1;
    inst.R.rs1   = 2;

    regs.x[inst.R.rs1] = ;
    regs.x[inst.R.rs2] = ;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(, regs.x[inst.R.rd]);
}
*/

TEST(STORE, SH)
{
    rv32::instS_imm imm;
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.S.func3 = 0b001;
    inst.S.rs1   = 1;
    inst.S.rs2   = 2;

    imm.imm            = 0;
    inst.S.imm11_5     = imm.imm11_5;
    inst.S.imm4_0      = imm.imm4_0;
    regs.x[inst.R.rs1] = 0x10000200;
    regs.x[inst.R.rs2] = 0x11223344;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0x00, test.m_mem->m_mem[0x1FF]);
    EXPECT_EQ(0x44, test.m_mem->m_mem[0x200]);
    EXPECT_EQ(0x33, test.m_mem->m_mem[0x201]);
    EXPECT_EQ(0x00, test.m_mem->m_mem[0x202]);

    tester.m_mem->reset();
    imm.imm            = -1;
    inst.S.imm11_5     = imm.imm11_5;
    inst.S.imm4_0      = imm.imm4_0;
    auto res2 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res2);
    EXPECT_EQ(0x00, test.m_mem->m_mem[0x1FE]);
    EXPECT_EQ(0x44, test.m_mem->m_mem[0x1FF]);
    EXPECT_EQ(0x33, test.m_mem->m_mem[0x200]);
    EXPECT_EQ(0x00, test.m_mem->m_mem[0x201]);
}

TEST(STORE, SB)
{
    rv32::instS_imm imm;
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.S.func3 = 0b000;
    inst.S.rs1   = 1;
    inst.S.rs2   = 2;

    imm.imm            = 0;
    inst.S.imm11_5     = imm.imm11_5;
    inst.S.imm4_0      = imm.imm4_0;
    regs.x[inst.R.rs1] = 0x10000200;
    regs.x[inst.R.rs2] = 0x11223344;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0x00, test.m_mem->m_mem[0x1FF]);
    EXPECT_EQ(0x44, test.m_mem->m_mem[0x200]);
    EXPECT_EQ(0x00, test.m_mem->m_mem[0x201]);

    tester.m_mem->reset();
    imm.imm            = -1;
    inst.S.imm11_5     = imm.imm11_5;
    inst.S.imm4_0      = imm.imm4_0;
    auto res2 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res2);
    EXPECT_EQ(0x00, test.m_mem->m_mem[0x1FE]);
    EXPECT_EQ(0x44, test.m_mem->m_mem[0x1FF]);
    EXPECT_EQ(0x00, test.m_mem->m_mem[0x200]);
}
