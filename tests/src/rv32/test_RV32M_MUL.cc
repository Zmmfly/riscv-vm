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

TEST(M_mul, mulhu)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.R.func3 = 0b011;
    inst.R.func7 = 0b000'0001;
    inst.R.rd    = 3;
    inst.R.rs1   = 1;
    inst.R.rs2   = 2;

    regs.x[inst.R.rs1] = 0xffffffff;
    regs.x[inst.R.rs2] = 0xfe;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0xfd, regs.x[inst.R.rd]);
}

TEST(M_mul, mulhsu)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.R.func3 = 0b010;
    inst.R.func7 = 0b000'0001;
    inst.R.rd    = 3;
    inst.R.rs1   = 1;
    inst.R.rs2   = 2;

    regs.x[inst.R.rs1] = 0x8000000;
    regs.x[inst.R.rs2] = -2;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0x80000001, regs.x[inst.R.rd]);
}

TEST(M_mul, mulh)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.R.func3 = 0b001;
    inst.R.func7 = 0b000'0001;
    inst.R.rd    = 3;
    inst.R.rs1   = 1;
    inst.R.rs2   = 2;

    regs.x[inst.R.rs1] = 0x8000000;
    regs.x[inst.R.rs2] = -2;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(1, regs.x[inst.R.rd]);
}

TEST(M_mul, mul)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.R.func3 = 0b000;
    inst.R.func7 = 0b000'0001;
    inst.R.rd    = 3;
    inst.R.rs1   = 1;
    inst.R.rs2   = 2;

    regs.x[inst.R.rs1] = -8;    /* FFFFFFF8 */
    regs.x[inst.R.rs2] = 3;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(-24, (int32_t)regs.x[inst.R.rd]);
}
