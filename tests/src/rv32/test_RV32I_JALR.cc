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
    inst.opcode = 0b1100111;
    inst.others = 0;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(JALR, JALR)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others        = 0;
    inst.I.func3       = 0b000;
    inst.I.rd          = 1;
    inst.I.rs1         = 2;
    inst.I.imm         = -5;
    regs.pc            = 12;
    regs.x[inst.I.rs1] = 12;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(16, regs.x[inst.R.rd]);
    EXPECT_EQ(6, regs.pc);
}
