#ifndef __ZMMFLY_RVVM_UTILS_H__
#define __ZMMFLY_RVVM_UTILS_H__

#include <cstdint>

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
T mul(const T a, const T b, T& out_high) {
    // 验证T确实是无符号类型
    static_assert(std::is_unsigned<T>::value, "T must be an unsigned type.");

    // 将输入数拆分为两半
    T mask   = static_cast<T>((T)1 << (sizeof(T) * 4)) - 1;  // 用于分割输入数的掩码
    T a_high = (a >> (sizeof(T) * 4));                       // a的高位部分
    T a_low  = a & mask;                                     // a的低位部分
    T b_high = (b >> (sizeof(T) * 4));                       // b的高位部分
    T b_low  = b & mask;                                     // b的低位部分

    // 计算直接乘积和交叉乘积
    T high_high = a_high * b_high;
    T high_low  = a_high * b_low;
    T low_high  = a_low * b_high;
    T low_low   = a_low * b_low;

    // 合并交叉乘积，处理可能的溢出
    T middle       = high_low + low_high;
    T middle_carry = middle < high_low ? 1 : 0;

    // 计算最终的高位和低位，注意溢出处理
    T low_part       = low_low + (middle << (sizeof(T) * 4));
    T carry_from_low = low_part < low_low ? 1 : 0;             // 检查从低位部分加法的溢出

    // 最终高位结果包括直接的高位乘积、中间部分的溢出以及低位加法的溢出
    out_high = high_high + (middle >> (sizeof(T) * 4)) + middle_carry + carry_from_low;

    // 返回低位部分作为乘积结果
    return low_part;
}

}

#endif /* __ZMMFLY_RVVM_UTILS_H__ */
