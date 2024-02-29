#ifndef __ZMMFLY_RV_INTF_H__
#define __ZMMFLY_RV_INTF_H__

#include "zmmfly/rv_defs.h"
#include "zmmfly/rv_base.h"

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
class bus_mgr_intf{
public:
    using listener_t      = std::function<void(T addr, T offset, void* ptr, size_t len)> ;
    using listener_info_t = std::tuple<T, size_t, listener_t> ;

    virtual ~bus_mgr_intf() {};

    /**
     * @brief Mount memory to bus
     * 
     * @param addr 
     * @param size 
     * @param mem 
     * @return rv_err_t 
     */
    virtual rv_err_t mount(T addr, size_t size, std::shared_ptr<bus_intf<T>> mem) = 0;

    /**
     * @brief Read memory from bus
     * 
     * @param addr 
     * @param ptr 
     * @param len 
     * @param arg
     * @return rv_err_t 
     */
    virtual rv_err_t read(T addr, void* ptr, size_t len, std::any arg = {})  = 0;

    /**
     * @brief Listen read
     * 
     * @param addr listen address of begin
     * @param len listen length
     * @param fn listen callback, the first arg is addr, second is ptr, third is size
     * @param listen_id listen id
     * @return rv_err_t 
     */
    virtual rv_err_t read_listen(T addr, size_t len, listener_t fn, T& listen_id) = 0;

    /**
     * @brief Unlisten read
     * 
     * @param listen_id 
     * @return rv_err_t 
     */
    virtual rv_err_t read_unlisten(T listen_id) = 0;

    /**
     * @brief Write memory to bus
     * 
     * @param addr 
     * @param ptr 
     * @param len
     * @param arg 
     * @return rv_err_t 
     */
    virtual rv_err_t write(T addr, void* ptr, size_t len, std::any arg = {}) = 0;

    virtual rv_err_t write_listen(T addr, size_t len, listener_t fn, T& listen_id) = 0;

    virtual rv_err_t write_unlisten(T listen_id) = 0;

    /**
     * @brief Set any value
     * 
     * @param k 
     * @param v 
     * @return rv_err_t 
     */
    virtual rv_err_t set(std::string k, std::any v) = 0;
};

template<typename T = uint32_t>
class inst_intf
{
public:
    using registers_t = registers<T>;
    using bus_intf_t  = bus_intf<T>;
    using inst_map_t  = std::map<std::string, std::shared_ptr<inst_intf<T>>>;
    using vcall_t     = std::function<void(T)>;

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
    virtual rv_err_t insts_notify(std::string insts) 
    {
        return RV_EUNSUPPORTED;
    }

    /**
     * @brief Set str:str kv
     * 
     * @param k 
     * @param v 
     * @return rv_err_t 
     */
    virtual rv_err_t set(std::string k, std::string v) 
    {
        return RV_EUNSUPPORTED;
    }

    /**
     * @brief Get str:str kv
     * 
     * @param k 
     * @param v use pointer as std::any to get value
     * @return rv_err_t 
     */
    virtual rv_err_t get(std::string k, std::string& v) 
    {
        return RV_EUNSUPPORTED; 
    }

    /**
     * @brief Set str:T kv
     * 
     * @param k 
     * @param v 
     * @return rv_err_t 
     */
    virtual rv_err_t set(std::string k, T v)
    {
        return RV_EUNSUPPORTED;
    }

    /**
     * @brief Get str:T kv
     * 
     * @param k 
     * @param v 
     * @return rv_err_t 
     */
    virtual rv_err_t get(std::string k, T& v)
    {
        return RV_EUNSUPPORTED;
    }

    /**
     * @brief Set t:t kv
     * 
     * @param k 
     * @param v 
     * @return rv_err_t 
     */
    virtual rv_err_t set(T k, T v)
    {
        return RV_EUNSUPPORTED;
    }

    /**
     * @brief Get t:t kv
     * 
     * @param k 
     * @param v 
     * @return rv_err_t 
     */
    virtual rv_err_t get(T k, T& v)
    {
        return RV_EUNSUPPORTED;
    }

    /**
     * @brief Set on instruction event callback, usually for special instruction
     * 
     * @param key 
     * @param fn 
     * @return rv_err_t 
     */
    virtual rv_err_t on(std::string key, vcall_t fn)
    {
        return RV_EUNSUPPORTED;
    }

    /**
     * @brief Normal instruction executor
     * 
     * @param inst instruction
     * @param regs registers refer
     * @param dbus data bus
     * @param inst_map
     * @return rv_err_t 
     */
    virtual rv_err_t execute_normal(T inst, registers_t& regs, bus_mgr_intf<T>& dbus, inst_map_t& inst_map) 
    {
        return RV_EUNSUPPORTED; 
    };

    /**
     * @brief Compression instruction executor
     * 
     * @param inst instruction
     * @param regs registers refer
     * @param dbus data bus
     * @param inst_map
     * @return rv_err_t 
     */
    virtual rv_err_t execute_compr(uint16_t inst, registers_t& regs, bus_mgr_intf<T>& dbus, inst_map_t& inst_map)
    {
        return RV_EUNSUPPORTED;
    };
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
