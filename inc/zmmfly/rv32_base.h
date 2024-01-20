#ifndef __ZMMFLY_RV32_INST_H__
#define __ZMMFLY_RV32_INST_H__

#include <cstdint>
#include "zmmfly/rv_intf.h"
#include "zmmfly/rv_busmgr.hpp"

namespace zmmfly::rv32
{

#pragma pack(push, 1)
using inst_type = union inst_type
{
    uint16_t u16[2];    /* could using in compression instruction */
    uint32_t u32;
    struct {
        uint32_t opcode: 7;
        uint32_t :25;
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
        uint32_t opcode: 7;
        uint32_t rd    : 5;
        uint32_t func3 : 3;
        uint32_t rs1   : 5;
        uint32_t imm   : 12;    /* imm[11:0] */
    }I;

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
        uint32_t imm12:1;
        uint32_t :20;
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

using bus_t       = zmmfly::rv::bus_mgr_intf<uint32_t>;
using bus_intf_t  = zmmfly::rv::bus_intf<uint32_t>;
using inst_intf_t = zmmfly::rv::inst_intf<uint32_t>;
#pragma pack(pop)

};

#endif /* __ZMMFLY_RV32_INST_H__ */
