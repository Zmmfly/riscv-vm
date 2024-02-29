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
    inst.opcode = 0b0110111;
    inst.others = 0;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(LUI, LUI)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others     = 0;
    inst.U.rd       = 1;
    inst.U.imm31_12 = -4;   /* -16384, 0xFFFFC000 */

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0xFFFFC000, regs.x[inst.U.rd]);
}
