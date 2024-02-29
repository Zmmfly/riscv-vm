#ifndef __ZMMFLY_RV_REGISTERS_H__
#define __ZMMFLY_RV_REGISTERS_H__

#include "zmmfly/rv_defs.h"

namespace zmmfly::rv
{

#pragma pack(push, 1)
template<typename T>
struct registers
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
#pragma pack(pop)

};

#endif /* __ZMMFLY_RV_REGISTERS_H__ */
