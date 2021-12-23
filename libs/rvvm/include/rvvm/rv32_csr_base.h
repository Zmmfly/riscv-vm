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
        union{
            uint32_t value;
            struct
            {
                uint32_t offset:7;
                uint32_t bank:25;
            };
        }m_mvendorid; //could be zero

        union{
            uint32_t value;
        }m_marchid; //could be zero

        union{
            uint32_t value;
        }m_mimpid;  //could be zero

        union{
            uint32_t value;
        }m_mhartid; //could be zero

        union{
            uint32_t value;
        }m_configptr; //could be zero

        union
        {
            uint32_t value;
            struct
            {
                uint32_t UIE:1;
                uint32_t SIE:1;
                uint32_t :1;
                uint32_t MIE:1;
                uint32_t UPIE:1;
                uint32_t SPIE:1;
                uint32_t :1;
                uint32_t MPIE:1;
                uint32_t SPP:1;
                uint32_t :2;
                uint32_t MPP:2;
                uint32_t FS:2;
                uint32_t XS:2;
                uint32_t :1;
                uint32_t SUM:1;
                uint32_t MXR:1;
                uint32_t TVM:1;
                uint32_t TW:1;
                uint32_t TSR:1;
                uint32_t :8;
                uint32_t SD:1;
            };
        }m_mstatus;

        union{
            uint32_t value;
            struct
            {
                uint32_t extensions:26;
                uint32_t :4;
                uint32_t mxl:2;
            };
        }m_misa; // could be zero

        union
        {
            uint32_t value;
        }m_medeleg; // only need in S-mode

        union
        {
            uint32_t value;
        }m_mideleg; // same with medeleg

        union{}m_mie;

        union{
            uint32_t value;
            struct
            {
                uint32_t mode:2;
                uint32_t base_31_2:30;
            };
        }m_mtvec;

        union{
            uint32_t value;
        }m_mcounteren; // only need in U-mode

        union{
            uint32_t value;
        }m_mscratch;

        union
        {
            uint32_t value;
        }m_mepc;

        union{
            uint32_t value;
            struct
            {
                uint32_t code:31;
                uint32_t interrupt:1;
            };
        }m_mcause;

        union
        {
            uint32_t value;
        }m_mtval;
        union{}m_mip;
        union{}m_mtinst;
        union{}m_tval2;
        union{}m_menvcfg;
        union{}m_menvcfgh;
        union{}m_mseccfg;
        union{}m_mseccfgh;
        union{}m_mcycle;
        union{}m_minstret;
        union{}m_mhpmcounter[30];
        union{}m_cycleh;
        union{}m_instreth;
        union{}m_mhpmcounterh[30];
        union{}m_mcountinhibit;
        union{}m_mhpmevent[30];
};

};

#endif