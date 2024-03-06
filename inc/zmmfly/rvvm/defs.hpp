#ifndef __ZMMFLY_RVVM_DEFS_H__
#define __ZMMFLY_RVVM_DEFS_H__

#include <cstdint>
#include <cstddef>
#include <memory>
#include <vector>
#include <tuple>
#include <map>
#include <string>

namespace Zmmfly::rvvm
{

using err_t = enum err_t
{
    RV_EOK = 0,
    RV_EILLEGAL_INST,
    RV_EUNALIGNED,
    RV_EUNSUPPORTED,
    RV_EOVERLAP,
    RV_EOVERFLOW,
    RV_ERANGE,
    RV_ETYPE,
    RV_EBUSFAULT,
    RV_EIO,
    RV_ENOTFOUND, 
};

#pragma pack(push, 1)
template<typename T>
struct regs_t
{
    static_assert(std::is_same<T, uint32_t>::value || std::is_same<T, uint64_t>::value,
                  "Template parameter T must be uint32_t or uint64_t");
    union {
        struct{
            T x[33];
            union{
                uint32_t u32[2];
                float f32[2];
                uint64_t u64;
                double f64;
            } f[32];
        };
        struct{
            T zero;      /* x0 [-]               : Hard-wired zero register */
            T ra;        /* x1 [Caller]          : Return address */
            T sp;        /* x2 [Callee]          : Stack pointer */
            T gp;        /* x3 [-]               : Global pointer */
            T tp;        /* x4 [-]               : Thread pointer */
            T t0[3];     /* x5 ~ x7 [Caller]     : Temporaries */
            T s0_fp;     /* x8 [Callee]          : Saved register | frame pointer */
            T s1;        /* x9 [Callee]          : Saved register */
            T a[8];      /* x10 ~ x17 [Caller]   : Function arguments | return values  */
            T s2[10];    /* x18 ~ x27 [Callee]   : s2 ~ s11 Saved registers */
            T t3[4];     /* x28 ~ x31 [Caller]   : t3 ~ t6 Temporaries */
            T pc;
            uint64_t fp64[32];
        };
    };
};

using inst_type = union inst_type
{
    uint16_t u16[2];    /* could using in compression instruction */
    uint32_t u32;
    struct {
        uint32_t opcode: 7;
        uint32_t others:25;
    };
    
    struct {
        uint32_t      : 7;
        uint32_t rd   : 5;
        uint32_t func3: 3;
        uint32_t      :17;
    };

    struct {
        uint32_t opcode: 7;
        uint32_t rd    : 5;
        uint32_t func3 : 3;
        uint32_t rs1   : 5;
        uint32_t SW    : 1;
        uint32_t SR    : 1;
        uint32_t SO    : 1;
        uint32_t SI    : 1;
        uint32_t PW    : 1;
        uint32_t PR    : 1;
        uint32_t PO    : 1;
        uint32_t PI    : 1;
        uint32_t fm    : 4;
    }fence;

    struct {
        uint32_t opcode: 7;
        uint32_t rd    : 5;
        uint32_t func3 : 3;
        uint32_t rs1   : 5;
        uint32_t rs2   : 5;
        uint32_t func7 : 7;
    }R;

    struct {
        uint32_t opcode   : 7;
        uint32_t rd       : 5;
        uint32_t rm       : 3;
        uint32_t rs1      : 5;
        uint32_t rs2      : 5;
        uint32_t fmt      : 2;
        uint32_t rs3_func5: 5;
    }R_fp;

    struct {
        uint32_t opcode: 7;
        uint32_t rd    : 5;
        uint32_t func3 : 3;
        uint32_t rs1   : 5;
        uint32_t rs2   : 5;
        uint32_t rl    : 1;
        uint32_t aq    : 1;
        uint32_t func5 : 5;
    }R_amo;

    struct {
        uint32_t opcode: 7;
        uint32_t rd    : 5;
        uint32_t func3 : 3;
        uint32_t rs1   : 5;
        uint32_t imm   : 12;    /* imm[11:0] */
    }I;

    struct {
        uint32_t opcode: 7;
        uint32_t rd    : 5;
        uint32_t func3 : 3;
        uint32_t rs1   : 5;
        uint32_t shamt : 5;    /* imm[4:0] */
        uint32_t func7 : 7;    /* imm[11:5] */
    }I_s5;

    struct {
        uint32_t opcode: 7;
        uint32_t rd    : 5;
        uint32_t func3 : 3;
        uint32_t rs1   : 5;
        uint32_t shamt : 6;    /* imm[4:0] */
        uint32_t func6 : 6;    /* imm[11:5] */
    }I_s6;

    struct {
        uint32_t opcode: 7;
        uint32_t rd    : 5;
        uint32_t width : 3;
        uint32_t rs1   : 5;
        uint32_t imm   : 12;    /* imm[11:0] */
    }I_fp;

    struct {
        uint32_t opcode : 7;
        uint32_t imm4_0 : 5;  /* imm[4:0] */
        uint32_t func3  : 3;
        uint32_t rs1    : 5;
        uint32_t rs2    : 5;
        uint32_t imm11_5: 7;  /* imm[11:5] */
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
        uint32_t opcode  : 7;
        uint32_t rd      : 5;
        uint32_t imm31_12: 20;  /* imm[31:12] */
    }U;

    struct {
        uint32_t opcode  : 7;
        uint32_t rd      : 5;
        uint32_t imm19_12: 8;   /* imm[19:12] */
        uint32_t immb11  : 1;   /* imm[11] */
        uint32_t imm10_1 : 10;  /* imm[10:1] */
        uint32_t immb20  : 1;   /* imm[20] */
    }J;
};

typedef union instB_imm{
    uint32_t imm;
    struct{
        uint32_t :1;
        uint32_t imm4_1:4;
        uint32_t imm10_5:6;
        uint32_t imm11:1;
        uint32_t imm12:1;
        uint32_t :19;
    };
}instB_imm;

typedef union instJ_imm{
    uint32_t imm;
    struct {
        uint32_t :1;
        uint32_t imm10_1:10;
        uint32_t imm11:1;
        uint32_t imm19_12:8;
        uint32_t imm20:1;
        uint32_t :11;
    };
}instJ_imm;

typedef union instS_imm{
    uint32_t imm;
    struct {
        uint32_t imm4_0 : 5;
        uint32_t imm11_5: 7;
        uint32_t :20;
    };
}instS_imm;

#pragma pack(pop)

}

#endif /* __ZMMFLY_RVVM_DEFS_H__ */
