#ifndef __RV32_INSTRUCTION_TYPEDEF_H__
#define __RV32_INSTRUCTION_TYPEDEF_H__

#include <stdint.h>

namespace rvvm
{
    typedef union rv32_ins_t
    {
        uint32_t value;
        struct
        {
            uint32_t opcode:7;
            union
            {
                struct
                {
                    uint32_t rd:5;
                    uint32_t funct3:3;
                    uint32_t rs1:5;
                    uint32_t rs2:5;
                    uint32_t funct7:7;
                }R;

                struct
                {
                    uint32_t rd:5;
                    uint32_t funct3:3;
                    uint32_t rs1:5;
                    uint32_t imm_i:12;
                }I;

                struct
                {
                    uint32_t imm_4_0:5;
                    uint32_t func3:3;
                    uint32_t rs1:5;
                    uint32_t rs2:5;
                    uint32_t imm_11_5:6;
                }S;

                struct
                {
                    uint32_t imm_11:1;
                    uint32_t imm_4_1:4;
                    uint32_t funct3:3;
                    uint32_t rs1:5;
                    uint32_t rs2:5;
                    uint32_t imm_10_5:6;
                    uint32_t imm_12:1;
                }B;

                struct
                {
                    uint32_t rd:5;
                    uint32_t imm_31_12:20;
                }U;

                struct
                {
                    uint32_t rd:5;
                    uint32_t imm_19_12:8;
                    uint32_t imm_11:1;
                    uint32_t imm_10_1:10;
                    uint32_t imm_20:1;
                }J;
            };
        };
    }rv32_ins_t;
};

#endif
