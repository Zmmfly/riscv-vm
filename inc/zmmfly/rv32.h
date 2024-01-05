#ifndef __ZMMFLY_RVVM_RV32_H__
#define __ZMMFLY_RVVM_RV32_H__

#include <cstdint>

namespace zmmfly::rv32
{

#pragma pack(push, 1)
using basic_registers = struct basic_registers
{
    union {
        struct{
            uint32_t x[32];
            uint32_t f[32];
        };
        struct{
            uint32_t zero;      /* x0 [-]               : Hard-wired zero register */
            uint32_t ra;        /* x1 [Caller]          : Return address */
            uint32_t sp;        /* x2 [Callee]          : Stack pointer */
            uint32_t gp;        /* x3 [-]               : Global pointer */
            uint32_t tp;        /* x4 [-]               : Thread pointer */
            uint32_t t0[3];     /* x5 ~ x7 [Caller]     : Temporaries */
            uint32_t s0_fp;     /* x8 [Callee]          : Saved register | frame pointer */
            uint32_t s1;        /* x9 [Callee]          : Saved register */
            uint32_t a[8];      /* x10 ~ x17 [Caller]   : Function arguments | return values  */
            uint32_t s2[10];    /* x18 ~ x27 [Callee]   : s2 ~ s11 Saved registers */
            uint32_t t3[4];     /* x28 ~ x31 [Caller]   : t3 ~ t6 Temporaries */
            uint32_t ft0[8];    /* f0 ~ f7 [Caller]     : ft0 ~ ft7 FP temporaries */
            uint32_t fs0[2];    /* f8 ~ f9 [Callee]     : fs0 ~ fs1 FP saved registers */
            uint32_t fa[8];     /* f10 ~ f17 [Caller]   : fa0 ~ fa7 FP arguments | return values */
            uint32_t fs2[10];   /* f18 ~ f27 [Callee]   : fs2 ~ fs11 FP saved registers */
            uint32_t ft8[4];    /* f28 ~ f31 [Caller]   : ft8 ~ ft11 FP temporaries */
        };
    };
};

using inst_type = union inst_type
{
    uint16_t u16[2];    /* could using in compression instruction */
    uint32_t u32;
    struct {
        uint32_t opcode: 7;
        uint32_t rd    : 5;
        uint32_t func3 : 3;
        uint32_t rs1   : 5;
        uint32_t rs2   : 5;
        uint32_t func7 : 7;
    }R;

    struct {
        uint32_t opcode: 7;
        uint32_t rd    : 5;
        uint32_t func3 : 3;
        uint32_t rs1   : 5;
        uint32_t imm   : 12;    /* imm[11:0] */
    }I;

    struct {
        uint32_t opcode: 7;
        uint32_t imm1  : 5;     /* imm[4:0] */
        uint32_t func3 : 3;
        uint32_t rs1   : 5;
        uint32_t rs2   : 5;
        uint32_t imm2  : 7;     /* imm[11:5] */
    }S;

    struct {
        uint32_t opcode : 7;
        uint32_t immb11 : 1;    /* imm[11] */
        uint32_t imm4_1 : 4;    /* imm[4:1] */
        uint32_t func3  : 3;
        uint32_t rs1    : 5;
        uint32_t rs2    : 5;
        uint32_t imm10_5: 6;    /* imm[10:5] */
        uint32_t immb12 : 1;    /* imm[12] */
    }B;

    struct {
        uint32_t opcode: 7;
        uint32_t rd    : 5;
        uint32_t imm   : 20;    /* imm[31:12] */
    }U;

    struct {
        uint32_t opcode  : 7;
        uint32_t rd      : 5;
        uint32_t imm19_12: 8;   /* imm[19:12] */
        uint32_t immb11  : 1;   /* imm[11] */
        uint32_t imm10_1 : 10;  /* imm[10:1] */
        uint32_t immb12  : 1;   /* imm[12] */
    }J;
};
#pragma pack(pop)

};

#endif /* __ZMMFLY_RVVM_RV32_H__ */
