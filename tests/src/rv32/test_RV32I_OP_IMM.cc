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
    inst.opcode = 0b0010011;
    inst.others = 0;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/*
TEST(OP_IMM, template)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.I.func3 = ;
    inst.I.rd    = 3;
    inst.I.rs1   = 1;
}
*/

TEST(OP_IMM, ANDI)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.I.func3 = 0b111;
    inst.I.rd    = 3;
    inst.I.rs1   = 1;

    regs.x[inst.I.rs1] = 0x55AAAA54;
    inst.I.imm         = 1;

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0, regs.x[inst.I.rd]);

    regs.x[inst.I.rs1] = 0xc00;
    inst.I.imm         = -1024;
    auto res2 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res2);
    EXPECT_EQ(0xc00, regs.x[inst.I.rd]);
}

TEST(OP_IMM, ORI)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.I.func3 = 0b110;
    inst.I.rd    = 3;
    inst.I.rs1   = 1;

    regs.x[inst.I.rs1] = 0xffff0000;
    inst.I.imm         = 0xff;
    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0xffff00ff, regs.x[inst.I.rd]) << "x[rs1] 0xffff0000 | 0xff fail";

    regs.x[inst.I.rs1] = 0x151;
    inst.I.imm         = -1024;
    auto res2 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res2);
    EXPECT_EQ(0xfffffd51, regs.x[inst.I.rd]) << "x[rs1] 0x151 | sext(-1024) fail";
}

TEST(OP_IMM, XORI)
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

    regs.x[inst.I.rs1] = 0;
    inst.I.imm         = -3;    /* 0xfffffffd */

    auto res1 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res1);
    EXPECT_EQ(0xfffffffd, regs.x[inst.I.rd]) << "-3(0xfffffffd) xor x[rs1] 0 fail";

    regs.x[inst.I.rs1] = 0x40000000;
    auto res2 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res2);
    EXPECT_EQ(0xbffffffd, regs.x[inst.I.rd]) << "-3(0xfffffffd) xor x[rs1] 0x40000000 fail";

    regs.x[inst.I.rs1] = 0;
    inst.I.imm         = 0b0101;
    auto res3 = test.exec(inst.u32);
    EXPECT_EQ(RV_EOK, res3);
    EXPECT_EQ(0b0101, regs.x[inst.I.rd]) << "5 xor x[rs1] 0 fail";
}

TEST(OP_IMM, SLTIU)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.I.func3 = 0b011;
    inst.I.rd    = 3;
    inst.I.rs1   = 1;

    /* x[rs1] < (unsigned) sext(positive imm) */
    {
        regs.x[inst.I.rs1] = 1;
        inst.I.imm         = 2;

        auto res1 = test.exec(inst.u32);

        EXPECT_EQ(RV_EOK, res1);
        EXPECT_EQ(1, regs.x[inst.I.rd]) << "x[rs1] < u sext(p imm) fail\n";
    }

    /* x[rs1] > (unsigned) sext(positive imm) */
    {
        regs.x[inst.I.rs1] = 2;
        inst.I.imm         = 1;

        auto res1 = test.exec(inst.u32);

        EXPECT_EQ(RV_EOK, res1);
        EXPECT_EQ(0, regs.x[inst.I.rd]) << "x[rs1] > u sext(p imm) fail\n";
    }

    /* x[rs1] < (unsigned) sext(negative imm) */
    {
        regs.x[inst.I.rs1] = 0xffffff80;    /* -128 */
        inst.I.imm         = -127;          /* 0xffffff81 */

        auto res1 = test.exec(inst.u32);

        EXPECT_EQ(RV_EOK, res1);
        EXPECT_EQ(1, regs.x[inst.I.rd]) << "x[rs1] < u sext(n imm) fail\n";
    }

    /* x[rs1] > (unsigned) sext(negative imm) */
    {
        regs.x[inst.I.rs1] = 0xffffff81;    /* -127 */
        inst.I.imm         = -128;          /* 0xffffff80 */

        auto res1 = test.exec(inst.u32);

        EXPECT_EQ(RV_EOK, res1);
        EXPECT_EQ(0, regs.x[inst.I.rd]) << "x[rs1] > u sext(n imm) fail\n";
    }
}

TEST(OP_IMM, SLTI)
{
    tester.reset();
    tester.m_mem->reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.I.func3 = 0b010;
    inst.I.rd    = 3;       /* rd = x1 */
    inst.I.rs1   = 1;

    /* positive x[rs1] < positive sext(imm) */
    {
        regs.x[inst.I.rs1] = 100;
        inst.I.imm         = 200;

        auto res1 = test.exec(inst.u32);
        EXPECT_EQ(RV_EOK, res1);
        EXPECT_EQ(1, regs.x[inst.I.rd]) << "p < p fail\n";
    }

    /* positive x[rs1] > positive sext(imm) */
    {
        regs.x[inst.I.rs1] = 200;
        inst.I.imm         = 100;

        auto res1 = test.exec(inst.u32);
        EXPECT_EQ(RV_EOK, res1);
        EXPECT_EQ(0, regs.x[inst.I.rd]) << "p > p fail\n";
    }

    /* negative x[rs1] < negative sext(imm) */
    {
        regs.x[inst.I.rs1] = -200;
        inst.I.imm         = -100;

        auto res2 = test.exec(inst.u32);
        EXPECT_EQ(RV_EOK, res2);
        EXPECT_EQ(1, regs.x[inst.I.rd]) << "n < n fail\n";
    }

    /* negative x[rs1] > negative sext(imm) */
    {
        regs.x[inst.I.rs1] = -100;
        inst.I.imm         = -200;

        auto res2 = test.exec(inst.u32);
        EXPECT_EQ(RV_EOK, res2);
        EXPECT_EQ(0, regs.x[inst.I.rd]) << "n > n fail\n";
    }
}

TEST(OP_IMM, ADDI)
{
    tester.m_mem->reset();
    tester.reset();

    auto& regs = tester.regs;
    auto& test = tester;
    auto  mem  = tester.m_mem;

    inst.others  = 0;
    inst.I.func3 = 0b000;
    inst.I.rs1   = 2;      /* rs1 is x[2] */
    regs.x[2]    = 1;      /* x[2] = 1 */

    // test positive
    {
        inst.I.rd  = 1;
        inst.I.imm = 1;
        auto res1 = test.exec(inst.u32);
        EXPECT_EQ(RV_EOK, res1);
        EXPECT_EQ(2, regs.x[inst.I.rd]) << "add positive fail\n";
    }

    // test negative
    {
        inst.I.rd  = 1;
        inst.I.imm = (-1);
        auto res2 = test.exec(inst.u32);
        EXPECT_EQ(RV_EOK, res2);
        EXPECT_EQ(0, regs.x[inst.I.rd]) << "add negative fail\n";
    }

    // test zero
    {
        inst.I.imm = 0;
        inst.I.rd  = 1;
        auto res3 = test.exec(inst.u32);
        EXPECT_EQ(RV_EOK, res3);
        EXPECT_EQ(1, regs.x[inst.I.rd]) << "add zero fail\n";
    }

    // test x0, this register must keep zero
    {
        inst.I.imm = 1;
        inst.I.rd  = 0;
        auto res4 = test.exec(inst.u32);
        EXPECT_EQ(RV_EOK, res4);
        EXPECT_EQ(0, regs.x[inst.I.rd]) << "dst x0 fail\n";
    }
}
