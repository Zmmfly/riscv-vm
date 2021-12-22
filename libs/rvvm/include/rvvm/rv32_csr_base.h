#ifndef __RV32_CSR_BASE_H__
#define __RV32_CSR_BASE_H__

#include <stdint.h>
#include <map>
#include "rvvm/rv32_csr_impl.h"
#include "rvvm/rv32_core.h"

namespace rvvm
{

class rv32_csr_base:public rv32_csr_impl
{
    public:
        rv32_csr_base();
        ~rv32_csr_base();
        bool had_addr(rv32_core *core, uint16_t addr);
        bool rd(rv32_core *core, uint16_t addr, uint32_t &out);
        bool wr(rv32_core *core, uint16_t addr, uint32_t in);

    private:
        std::map<uint16_t, uint32_t &> m_collect;
};

};

#endif