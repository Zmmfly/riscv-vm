#include "rvvm/rv32_instruction_impl.h"

class RV32I: public rvvm::rv32_instruction_impl
{
    public:
        RV32I();
        ~RV32I();
        bool had_instruction(rvvm::rv32_ins_t ins);
        std::string name();
        bool execute(rvvm::rv32_registers &regs, rvvm::rv32_periph_collect &periphs, rvvm::rv32_ins_t ins, uint32_t &used_cycle);

    private:
        #define idef(name) void i_##name(rvvm::rv32_registers &regs, rvvm::rv32_periph_collect &periphs, rvvm::rv32_ins_t ins, uint32_t &used_cycle)
        #include "RV32I_instreuction"
        #undef idef
};
