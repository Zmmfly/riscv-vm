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
    inst.opcode = 0b0001111;
    inst.others = 0;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(MISC_MEM, FENCE)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others      = 0;
    inst.fence.func3 = 0b000;
    inst.fence.rd    = 0;
    inst.fence.rs1   = 0;

    bool called = false;

    test.instmap["I"]->set("fence", zmmfly::rv32::v1::I::ext_call_t([&called](zmmfly::rv32::inst_type inst){
        called = true;
    }));

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(true, called);
}
