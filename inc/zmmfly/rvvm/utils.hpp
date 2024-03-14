#ifndef __ZMMFLY_RVVM_UTILS_H__
#define __ZMMFLY_RVVM_UTILS_H__

#include <cstdint>
#include "Zmmfly/rvvm/defs.hpp"

namespace Zmmfly::rvvm
{

template<typename T>
T sext(T imm, uint8_t width) {
    static_assert(std::is_same<T, int32_t>::value || std::is_same<T, int64_t>::value,
        "Template parameter T must be int32_t or int64_t");
    
    if (width >= sizeof(T) * 8) {
        // No need for sign extension if width is greater or equal to the type size
        return imm;
    }

    // Calculate the shift amount to sign extend the immediate value
    int shift = sizeof(T) * 8 - width;

    // Perform sign extension
    return (imm << shift) >> shift;
}

template<typename T>
void mul_unsigned(T a, T b, T& low, T& high) 
{
    // 验证T确实是无符号类型
    static_assert(std::is_unsigned<T>::value, "T must be an unsigned type.");

    // 将输入数拆分为两半
    T mask   = static_cast<T>( ~T(0) >> sizeof(T)*4 );  // 用于分割输入数的掩码
    T a_high = (a >> (sizeof(T) * 4));                  // a的高位部分
    T a_low  = a & mask;                                // a的低位部分
    T b_high = (b >> (sizeof(T) * 4));                  // b的高位部分
    T b_low  = b & mask;                                // b的低位部分

    // 计算直接乘积和交叉乘积
    T albl = a_low * b_low;
    T albh = a_low * b_high;
    T ahbl = a_high * b_low;
    T ahbh = a_high * b_high;

    // 计算中间部分的和，注意这里不直接加albl，而是分别加其高位部分到 albl 和 ahbl 上
    T middle_sum = (albh & mask) + (ahbl & mask) + (albl >> (sizeof(T) * 4));

    // 最终高位结果包括直接的高位乘积、中间部分的溢出以及低位加法的溢出
    low = albl + ( (albh & mask) << (sizeof(T) * 4)) + ( (ahbl & mask) << (sizeof(T) * 4) );
    high = ahbh + (albh >> (sizeof(T) * 4)) + ( ahbl >> (sizeof(T) * 4) ) + (middle_sum >> (sizeof(T) * 4));
}

template<typename T, typename T_out>
void mul_signed(T a, T b, T_out& low, T_out& high) {
    static_assert(std::is_signed<T>::value, "T must be a signed type.");

    using U = typename std::make_unsigned<T>::type;

    bool negative = (a < 0) != (b < 0);
    U ua = (a < 0) ? static_cast<U>(T(-1) * a) : a;
    U ub = (b < 0) ? static_cast<U>(T(-1) * b) : b;

    U u_low, u_high;
    mul_unsigned(ua, ub, u_low, u_high); // 假设此函数正确实现无符号乘法

    if (negative) {
        // 如果结果为负，我们取低位的补码
        u_low = ~u_low + 1;
        u_high = ~u_high + (u_low == 0 ? 1 : 0);
    }

    low = reinterpret_cast<T_out>(u_low);
    high = reinterpret_cast<T_out>(u_high);
}

template<typename T_signed, typename T_unsigned>
void mul_signed_unsigned(T_signed a, T_unsigned b, T_signed& low, T_signed& high) {
    static_assert(std::is_unsigned<T_unsigned>::value, "T_unsigned must be an unsigned type.");
    static_assert(std::is_signed<T_signed>::value, "T_signed must be a signed type.");

    using U = typename std::make_unsigned<T_signed>::type;

    bool negative = (a < 0);
    U ua = (a < 0) ? static_cast<U>(-a) : static_cast<U>(a);
    U ub = b; // No conversion needed, b is already unsigned

    U u_low, u_high;
    mul_unsigned(ua, ub, u_low, u_high); // 使用无符号乘法函数

    if (negative) {
        // 如果结果为负，取补码
        u_low = ~u_low + 1;
        u_high = ~u_high + (u_low == 0 ? 1 : 0);
    }

    // 强制转换无符号结果到有符号类型
    low = static_cast<T_signed>(u_low);
    high = static_cast<T_signed>(u_high);
}

typedef enum fp_class_t
{
    F_NINF = 0, /* Negative infinite */
    F_NNORM,    /* Negative normal */
    F_NSUBNORM, /* Negative sub normal */
    F_NZERO,    /* Negative zero */
    F_PZERO,    /* Positive zero */
    F_PSUBNORM, /* Positive sub normal */
    F_PNORM,    /* Positive normal */
    F_PINF,     /* Postive infinite */
    F_SNAN,     /* Signaling NaN */
    F_QNAN,     /* Quiet NaN */
}fp_class_t;

template<typename T>
fp_class_t fclass(T n)
{
    static_assert(std::is_same<T, uint32_t>::value || std::is_same<T, uint64_t>::value,
    "Template parameter T must be uint32_t or uint64_t");
    T e = 0, m = 0, e_msk = 0, m_msk = 0;
    if constexpr(sizeof(T) == 4) {
        auto &ref = reinterpret_cast<fp32_t&>(n);
        e_msk = ((1<<8) - 1);
        m_msk = ((1<<23) - 1);
    }
    else if constexpr(sizeof(T) == 8) {
        auto &ref = reinterpret_cast<fp64_t&>(n);
        e_msk = ((1<<11) - 1);
        m_msk = ((1<<52) - 1);
    }
}

}

#endif /* __ZMMFLY_RVVM_UTILS_H__ */
