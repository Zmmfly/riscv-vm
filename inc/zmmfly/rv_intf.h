#ifndef __ZMMFLY_RV_INTF_H__
#define __ZMMFLY_RV_INTF_H__

#include "zmmfly/rv_defs.h"
#include "zmmfly/rv_registers.h"

namespace zmmfly::rv
{

template<typename T>
class bus_intf
{

public:
    virtual ~bus_intf() {};

    virtual rv_err_t read(T offset, void* ptr, size_t len)  = 0;
    virtual rv_err_t write(T offset, void* ptr, size_t len) = 0;
};

/**
 * @brief Bus interface
 * 
 * @tparam T CPU width type, uint32_t or uint64_t
 */
template<typename T>
class bus_mgr{
public:
    virtual ~bus() {};

    virtual rv_err_t mount(T addr, size_t size, std::shared_ptr<bus_intf<T>> intf) = 0;

    virtual rv_err_t read(T offset, void* ptr, size_t len)  = 0;
    virtual rv_err_t write(T offset, void* ptr, size_t len) = 0;
};

template<typename T = uint32_t>
class inst_intf
{
public:
    using registers_t = typename struct registers<T>;
    using bus_intf_t  = typename bus_intf<T>;
    virtual ~inst_intf() {};

    /**
     * @brief Get instruction name, like I / M / A / F / D / C
     * 
     * @return std::string 
     */
    virtual std::string name() = 0;

    /**
     * @brief Instructions registed notify
     * @note This could using for Zicsr extension
     * 
     * @param insts instruction string, like IMAFDZicsrZifenceiC
     * @return rv_err_t 
     */
    virtual rv_err_t insts_notify(std::string insts)                    { return RV_EUNSUPPORTED; };

    /**
     * @brief Normal instruction executor
     * 
     * @param inst instruction
     * @param regs registers refer
     * @return rv_err_t 
     */
    virtual rv_err_t execute_normal(T inst, registers_t& regs, bus_mgr<T>& bus)       { return RV_EUNSUPPORTED; };

    /**
     * @brief Compression instruction executor
     * 
     * @param inst instruction
     * @param regs registers refer
     * @return rv_err_t 
     */
    virtual rv_err_t execute_compr(uint16_t inst, registers_t& regs, bus_mgr<T>& bus) { return RV_EUNSUPPORTED; };
};

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

};

#endif /* __ZMMFLY_RV_INTF_H__ */
