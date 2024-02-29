#ifndef __ZMMFLY_RV32_FP_H__
#define __ZMMFLY_RV32_FP_H__

#include "zmmfly/rv32_base.h"

namespace zmmfly::rv32
{

using fp_type = enum fp_type 
{
    FP32  = 0b00,
    FP64  = 0b01,
    FP16  = 0b10,
    FP128 = 0b11,
};

class FP
: public inst_intf_t
{

public:
    FP(fp_type t = FP32);

    std::string name();

    rv_err_t execute_normal(uint32_t inst, registers_t& regs, bus_t& bus, inst_map_t& inst_map);

private:
    fp_type m_type;
};

};

#endif /* __ZMMFLY_RV32_F_H__ */
