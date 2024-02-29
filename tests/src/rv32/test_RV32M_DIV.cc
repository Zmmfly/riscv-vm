#include <cstdio>
#include <iostream>
#include "zmmfly/rv32I.h"
#include "tester_rv32i.h"
#include <gtest/gtest.h>
#include <fmt/core.h>

tester_rv32i tester;
zmmfly::rv::inst_type inst;

int main(int argc, char** argv)
{
    inst.opcode = 0b0110011;
    inst.others = 0;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(M_div, remu)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.R.func3 = 0b111;
    inst.R.func7 = 0b000'0001;
    inst.R.rd    = 3;
    inst.R.rs1   = 1;
    inst.R.rs2   = 2;

    regs.x[inst.R.rs1] = -8;    /* FFFFFFF8 */
    regs.x[inst.R.rs2] = 3;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(2, regs.x[inst.R.rd]);
}

TEST(M_div, rem)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.R.func3 = 0b110;
    inst.R.func7 = 0b000'0001;
    inst.R.rd    = 3;
    inst.R.rs1   = 1;
    inst.R.rs2   = 2;

    regs.x[inst.R.rs1] = -8;
    regs.x[inst.R.rs2] = -3;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(-2, (int32_t)regs.x[inst.R.rd]);
}

TEST(M_div, divu)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.R.func3 = 0b101;
    inst.R.func7 = 0b000'0001;
    inst.R.rd    = 3;
    inst.R.rs1   = 1;
    inst.R.rs2   = 2;

    regs.x[inst.R.rs1] = -8;    /* FFFFFFF8 */
    regs.x[inst.R.rs2] = 2;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0x7FFFFFFC, regs.x[inst.R.rd]);
}

TEST(M_div, div)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.R.func3 = 0b100;
    inst.R.func7 = 0b000'0001;
    inst.R.rd    = 3;
    inst.R.rs1   = 1;
    inst.R.rs2   = 2;

    regs.x[inst.R.rs1] = -8;
    regs.x[inst.R.rs2] = -2;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(4, regs.x[inst.R.rd]);
}
