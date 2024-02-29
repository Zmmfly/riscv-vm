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
    inst.opcode = 0b0010111;
    inst.others = 0;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(AUIPC, AUIPC)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others     = 0;
    inst.U.rd       = 1;
    inst.U.imm31_12 = 7;
    regs.pc         = 12;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0x700c, regs.x[inst.U.rd]) << "auipc 0x7000 fail\n";

    inst.U.imm31_12 = 0x98007;  /* 0x9800700 >> 12 */
    auto res2 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res2);
    EXPECT_EQ(0x9800700c, regs.x[inst.U.rd]) << "auipc 0x98007 fail\n";
}
