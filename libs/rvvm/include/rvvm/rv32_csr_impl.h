#ifndef __RV32_CSR_IMPL_H__
#define __RV32_CSR_IMPL_H__

namespace rvvm
{

class rv32_core;

class rv32_csr_impl
{
    public:
        virtual bool had_addr(rv32_core *core, uint32_t addr)          = 0;
        virtual bool rd(rv32_core *core, uint32_t addr, uint32_t &out) = 0;
        virtual bool wr(rv32_core *core, uint32_t addr, uint32_t in)   = 0;
};

class rv32_csr_collect
{
    public:
        rv32_csr_collect();
        ~rv32_csr_collect();

        void add(rv32_csr_impl *csr);
        bool had_addr(rv32_core *core, uint32_t addr);
        bool had_error();
        bool rd(rv32_core *core, uint32_t addr, uint32_t &out);
        bool wr(rv32_core *core, uint32_t addr, uint32_t in);
};

};

#endif