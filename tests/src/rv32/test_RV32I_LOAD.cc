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
    inst.opcode = 0b0000011;
    inst.others = 0;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(LOAD, LHU)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.I.func3 = 0b101;
    inst.I.rd    = 3;
    inst.I.rs1   = 1;

    test.m_mem->m_mem[0x1ff] = 0x11;
    test.m_mem->m_mem[0x200] = 0x22;
    test.m_mem->m_mem[0x201] = 0x33;
    test.m_mem->m_mem[0x202] = 0x44;
    test.m_mem->m_mem[0x203] = 0x55;
    test.m_mem->m_mem[0x204] = 0x66;
    test.m_mem->m_mem[0x205] = 0x77;
    test.m_mem->m_mem[0x206] = 0x88;
    test.m_mem->m_mem[0x207] = 0x99;
    test.m_mem->m_mem[0x208] = 0xaa;
    test.m_mem->m_mem[0x209] = 0xbb;

    regs.x[inst.I.rs1] = 0x10000200;  /* base addr */
    inst.I.imm         = -1;          /* offset */

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0x2211, regs.x[inst.I.rd]);

    test.m_mem->m_mem[0x200] = 0x82;
    auto res2 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0x8211, regs.x[inst.I.rd]);
}

TEST(LOAD, LBU)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.I.func3 = 0b100;
    inst.I.rd    = 3;
    inst.I.rs1   = 1;

    test.m_mem->m_mem[0x1ff] = 0x11;
    test.m_mem->m_mem[0x200] = 0x22;
    test.m_mem->m_mem[0x201] = 0x33;
    test.m_mem->m_mem[0x202] = 0x44;
    test.m_mem->m_mem[0x203] = 0x55;
    test.m_mem->m_mem[0x204] = 0x66;
    test.m_mem->m_mem[0x205] = 0x77;
    test.m_mem->m_mem[0x206] = 0x88;
    test.m_mem->m_mem[0x207] = 0x99;
    test.m_mem->m_mem[0x208] = 0xaa;
    test.m_mem->m_mem[0x209] = 0xbb;

    regs.x[inst.I.rs1] = 0x10000200;  /* base addr */
    inst.I.imm         = -1;          /* offset */

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0x11, regs.x[inst.I.rd]);

    test.m_mem->m_mem[0x1ff] = 0x81;
    auto res2 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0x81, regs.x[inst.I.rd]);
}

TEST(LOAD, LW)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.I.func3 = 0b010;
    inst.I.rd    = 3;
    inst.I.rs1   = 1;

    test.m_mem->m_mem[0x1ff] = 0x11;
    test.m_mem->m_mem[0x200] = 0x22;
    test.m_mem->m_mem[0x201] = 0x33;
    test.m_mem->m_mem[0x202] = 0x44;
    test.m_mem->m_mem[0x203] = 0x55;
    test.m_mem->m_mem[0x204] = 0x66;
    test.m_mem->m_mem[0x205] = 0x77;
    test.m_mem->m_mem[0x206] = 0x88;
    test.m_mem->m_mem[0x207] = 0x99;
    test.m_mem->m_mem[0x208] = 0xaa;
    test.m_mem->m_mem[0x209] = 0xbb;

    regs.x[inst.I.rs1] = 0x10000200;  /* base addr */
    inst.I.imm         = -1;          /* offset */

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0x44332211, regs.x[inst.I.rd]);

    test.m_mem->m_mem[0x202] = 0x84;
    auto res2 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0x84332211, regs.x[inst.I.rd]);
}

TEST(LOAD, LH)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.I.func3 = 0b001;
    inst.I.rd    = 3;
    inst.I.rs1   = 1;

    test.m_mem->m_mem[0x1ff] = 0x11;
    test.m_mem->m_mem[0x200] = 0x22;
    test.m_mem->m_mem[0x201] = 0x33;
    test.m_mem->m_mem[0x202] = 0x44;
    test.m_mem->m_mem[0x203] = 0x55;
    test.m_mem->m_mem[0x204] = 0x66;
    test.m_mem->m_mem[0x205] = 0x77;
    test.m_mem->m_mem[0x206] = 0x88;
    test.m_mem->m_mem[0x207] = 0x99;
    test.m_mem->m_mem[0x208] = 0xaa;
    test.m_mem->m_mem[0x209] = 0xbb;

    regs.x[inst.I.rs1] = 0x10000200;  /* base addr */
    inst.I.imm         = -1;          /* offset */

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0x2211, regs.x[inst.I.rd]);

    test.m_mem->m_mem[0x200] = 0x82;
    auto res2 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0xffff8211, regs.x[inst.I.rd]);
}

TEST(LOAD, LB)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.I.func3 = 0b000;
    inst.I.rd    = 3;
    inst.I.rs1   = 1;

    test.m_mem->m_mem[0x1ff] = 0x11;
    test.m_mem->m_mem[0x200] = 0x22;
    test.m_mem->m_mem[0x201] = 0x33;
    test.m_mem->m_mem[0x202] = 0x44;
    test.m_mem->m_mem[0x203] = 0x55;
    test.m_mem->m_mem[0x204] = 0x66;
    test.m_mem->m_mem[0x205] = 0x77;
    test.m_mem->m_mem[0x206] = 0x88;
    test.m_mem->m_mem[0x207] = 0x99;
    test.m_mem->m_mem[0x208] = 0xaa;
    test.m_mem->m_mem[0x209] = 0xbb;

    regs.x[inst.I.rs1] = 0x10000200;  /* base addr */
    inst.I.imm         = -1;          /* offset */

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0x11, regs.x[inst.I.rd]);

    test.m_mem->m_mem[0x1ff] = 0x81;
    auto res2 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res2);
    EXPECT_EQ(0xFFFFFF81, regs.x[inst.I.rd]);
}
