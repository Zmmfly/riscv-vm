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
    inst.opcode = 0b0110011;
    inst.others = 0;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/*
TEST(OP, template)
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

TEST(OP, AND)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.R.func3 = 0b111;
    inst.R.func7 = 0b0000000;
    inst.R.rd    = 3;
    inst.R.rs1   = 1;
    inst.R.rs1   = 2;

    regs.x[inst.R.rs1] = 0x80000000;
    regs.x[inst.R.rs2] = 0x80000008;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0x80000000, regs.x[inst.R.rd]);
}

TEST(OP, OR)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.R.func3 = 0b110;
    inst.R.func7 = 0b0000000;
    inst.R.rd    = 3;
    inst.R.rs1   = 1;
    inst.R.rs1   = 2;

    regs.x[inst.R.rs1] = 0x80000000;
    regs.x[inst.R.rs2] = 0x00000008;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0x80000008, regs.x[inst.R.rd]);
}

TEST(OP, SRA)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.R.func3 = 0b101;
    inst.R.func7 = 0b0100000;
    inst.R.rd    = 3;
    inst.R.rs1   = 1;
    inst.R.rs1   = 2;

    regs.x[inst.R.rs1] = -1024; /* 0xfffffc00 */
    regs.x[inst.R.rs2] = 3;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0xffffff80, regs.x[inst.R.rd]);
}

TEST(OP, SRL)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.R.func3 = 0b101;
    inst.R.func7 = 0b0000000;
    inst.R.rd    = 3;
    inst.R.rs1   = 1;
    inst.R.rs1   = 2;

    regs.x[inst.R.rs1] = 0x00000080;
    regs.x[inst.R.rs2] = 3;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(1, regs.x[inst.R.rd]);
}

TEST(OP, XOR)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.R.func3 = 0b100;
    inst.R.func7 = 0b0000000;
    inst.R.rd    = 3;
    inst.R.rs1   = 1;
    inst.R.rs1   = 2;

    regs.x[inst.R.rs1] = 0xfffffffe;
    regs.x[inst.R.rs2] = 0x5a5a5a5a;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0xa5a5a5a4, regs.x[inst.R.rd]);
}

TEST(OP, SLTU)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.R.func3 = 0b011;
    inst.R.func7 = 0b0000000;
    inst.R.rd    = 3;
    inst.R.rs1   = 1;
    inst.R.rs2   = 2;

    regs.x[inst.R.rs1] = -3;    /* 0xfffffffd */
    regs.x[inst.R.rs2] = -2;    /* 0xfffffffe */

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(1, regs.x[inst.R.rd]);

    regs.x[inst.R.rs1] = -2;
    regs.x[inst.R.rs2] = -3;

    auto res2 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res2);
    EXPECT_EQ(0, regs.x[inst.R.rd]);
}

TEST(OP, SLT)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.R.func3 = 0b010;
    inst.R.func7 = 0b0000000;
    inst.R.rd    = 3;
    inst.R.rs1   = 1;
    inst.R.rs2   = 2;

    regs.x[inst.R.rs1] = -3;
    regs.x[inst.R.rs2] = -2;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(1, regs.x[inst.R.rd]);

    regs.x[inst.R.rs1] = -2;
    regs.x[inst.R.rs2] = -3;

    auto res2 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res2);
    EXPECT_EQ(0, regs.x[inst.R.rd]);
}

TEST(OP, SLL)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.R.func3 = 0b001;
    inst.R.func7 = 0b0000000;
    inst.R.rd    = 3;
    inst.R.rs1   = 1;
    inst.R.rs2   = 2;

    regs.x[inst.R.rs1] = 4;
    regs.x[inst.R.rs2] = 1;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(8, regs.x[inst.R.rd]);
}

TEST(OP, SUB)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.R.func3 = 0b000;
    inst.R.func7 = 0b0100000;
    inst.R.rd    = 3;
    inst.R.rs1   = 1;
    inst.R.rs2   = 2;

    regs.x[inst.R.rs1] = 2;
    regs.x[inst.R.rs2] = 1;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(1, regs.x[inst.R.rd]);
}

TEST(OP, ADD)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.R.func3 = 0b000;
    inst.R.func7 = 0b0000000;
    inst.R.rd    = 3;
    inst.R.rs1   = 1;
    inst.R.rs2   = 2;

    regs.x[inst.R.rs1] = 1;
    regs.x[inst.R.rs2] = 1;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(2, regs.x[inst.R.rd]);
}
