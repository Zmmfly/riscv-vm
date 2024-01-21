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
TEST(JAL, template)
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

TEST(JAL, JAL)
{
    rv32::instJ_imm imm;
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    imm.imm         = -4;
    inst.others     = 0;
    inst.J.rd       = 1;
    inst.J.immb20   = imm.imm20;
    inst.J.imm19_12 = imm.imm19_12;
    inst.J.imm10_1  = imm.imm10_1;
    regs.pc         = 8;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(12, regs.x[inst.R.rd]);
    EXPECT_EQ(4, regs.pc);
}