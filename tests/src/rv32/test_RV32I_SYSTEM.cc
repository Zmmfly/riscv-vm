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
    inst.opcode = 0b1110011;
    inst.others = 0;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(SYSTEM, ECALL)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.I.rd    = 0;
    inst.I.func3 = 0;
    inst.I.rs1   = 0;
    inst.I.imm   = 0;

    bool called = false;

    test.instmap["I"]->on("ecall", zmmfly::rv32::inst_intf_t::vcall_t([&called](uint32_t inst){
        called = true;
    }));

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(true, called);
}

TEST(SYSTEM, EBREAK)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.I.rd    = 0;
    inst.I.func3 = 0;
    inst.I.rs1   = 0;
    inst.I.imm   = 1;

    bool called = false;

    test.instmap["I"]->on("ebreak", zmmfly::rv32::inst_intf_t::vcall_t([&called](uint32_t inst){
        called = true;
    }));

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(true, called);
}
