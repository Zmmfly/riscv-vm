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
    inst.opcode = 0b1100011;
    inst.others = 0;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/*
TEST(BRANCH, template)
{
    rv32::instB_imm v;
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    v.imm          = -4;
    inst.others    = 0;
    inst.B.func3   = ;
    inst.B.rs1     = 1;
    inst.B.rs2     = 2;
    inst.B.immb12  = v.imm12;
    inst.B.immb11  = v.imm11;
    inst.B.imm10_5 = v.imm10_5;
    inst.B.imm4_1  = v.imm4_1;

    regs.pc            = 12;
    regs.x[inst.B.rs1] = ;
    regs.x[inst.B.rs2] = ;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(8, regs.pc) << ;
}
*/

TEST(BRANCH, BGE)
{
    rv32::instB_imm v;
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    v.imm          = -4;
    inst.others    = 0;
    inst.B.func3   = 0b101;
    inst.B.rs1     = 1;
    inst.B.rs2     = 2;
    inst.B.immb12  = v.imm12;
    inst.B.immb11  = v.imm11;
    inst.B.imm10_5 = v.imm10_5;
    inst.B.imm4_1  = v.imm4_1;

    regs.pc            = 12;
    regs.x[inst.B.rs1] = -4;
    regs.x[inst.B.rs2] = -4;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(8, regs.pc) << "BGE >= fail";
}

TEST(BRANCH, BLT)
{
    rv32::instB_imm v;
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    v.imm          = -4;
    inst.others    = 0;
    inst.B.func3   = 0b100;
    inst.B.rs1     = 1;
    inst.B.rs2     = 2;
    inst.B.immb12  = v.imm12;
    inst.B.immb11  = v.imm11;
    inst.B.imm10_5 = v.imm10_5;
    inst.B.imm4_1  = v.imm4_1;

    regs.pc            = 12;
    regs.x[inst.B.rs1] = -5;
    regs.x[inst.B.rs2] = -4;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(8, regs.pc) << "BLT less fail\n";
}

TEST(BRANCH, BNE)
{
    rv32::instB_imm v;
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    v.imm          = -4;
    inst.others    = 0;
    inst.B.func3   = 0b001;
    inst.B.rs1     = 1;
    inst.B.rs2     = 2;
    inst.B.immb12  = v.imm12;
    inst.B.immb11  = v.imm11;
    inst.B.imm10_5 = v.imm10_5;
    inst.B.imm4_1  = v.imm4_1;

    regs.pc            = 12;
    regs.x[inst.B.rs1] = 5;
    regs.x[inst.B.rs2] = 4;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(8, regs.pc) << "BNE neq fail\n";

    regs.pc            = 12;
    regs.x[inst.B.rs1] = 5;
    regs.x[inst.B.rs2] = 5;

    auto res2 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res2);
    EXPECT_EQ(12, regs.pc) << "BNE eq fail\n";
}

TEST(BRANCH, BEQ)
{
    rv32::instB_imm v;
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others     = 0;
    inst.B.func3    = 0b000;
    inst.B.rs1      = 1;
    inst.B.rs2      = 2;

    regs.pc            = 12;
    regs.x[inst.B.rs1] = 0;
    regs.x[inst.B.rs2] = 0;
    v.imm              = -4;
    inst.B.immb12      = v.imm12;
    inst.B.immb11      = v.imm11;
    inst.B.imm10_5     = v.imm10_5;
    inst.B.imm4_1      = v.imm4_1;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(8, regs.pc) << "BEQ eq fail\n";

    regs.pc            = 12;
    regs.x[inst.B.rs1] = 0;
    regs.x[inst.B.rs2] = 1;
    auto res2 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res2);
    EXPECT_EQ(12, regs.pc) << "BEQ neq fail\n";
}
