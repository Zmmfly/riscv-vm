#ifndef __ZMMFLY_RVVM_INTF_H__
#define __ZMMFLY_RVVM_INTF_H__

#include "zmmfly/rvvm/defs.hpp"

namespace Zmmfly::rvvm
{

/**
 * @brief Memory interface
 * 
 * @tparam T CPU width type, uint32_t or uint64_t
 */
template<typename T>
class mem_intf
{
public:
    virtual ~mem_intf() {};

    /**
     * @brief Memory read
     * 
     * @param offset memory offset of current memory block
     * @param ptr buffer pointer
     * @param len read length
     * @return err_t 
     */
    virtual err_t read(T offset, void* ptr, T len)  = 0;

    /**
     * @brief Memory write
     * 
     * @param offset memory offset of current memory block
     * @param ptr buffer pointer
     * @param len write length
     * @return err_t 
     */
    virtual err_t write(T offset, void* ptr, T len) = 0;
};

/**
 * @brief Bus interface
 * 
 * @tparam T CPU width type, uint32_t or uint64_t
 */
template<typename T>
class bus_intf{
public:
    using listen_call_t = std::function<bool(T addr, void* ptr, T len, T last_listen_id, bool last_hooked, bool before_op)>;
    using listen_info_t = std::tuple<T, T, listen_call_t>;

    virtual ~bus_intf() {};

    /**
     * @brief Mount memory to bus
     * 
     * @param addr 
     * @param size 
     * @param mem 
     * @return err_t 
     */
    virtual err_t mount(T addr, T size, std::shared_ptr<mem_intf<T>> mem) = 0;

    /**
     * @brief Unmount mem by addr and size
     * 
     * @param addr 
     * @param size 
     * @return err_t 
     */
    virtual err_t unmount(T addr, T size) = 0;

    /**
     * @brief Unmount mem by mem intf
     * 
     * @return err_t 
     */
    virtual err_t unmount(std::shared_ptr<mem_intf<T>> mem) = 0;

    virtual err_t listen_read(T addr, T size, listen_call_t call, T& listen_id)
    {
        return RV_EUNSUPPORTED;
    }

    virtual err_t listen_write(T addr, T size, listen_call_t call, T& listen_id)
    {
        return RV_EUNSUPPORTED;
    }

    virtual err_t unlisten_read(T listen_id)
    {
        return RV_EUNSUPPORTED;
    }

    virtual err_t unlisten_read(T addr, T size)
    {
        return RV_EUNSUPPORTED;
    }

    virtual err_t unlisten_write(T listen_id)
    {
        return RV_EUNSUPPORTED;
    }

    virtual err_t unlisten_write(T addr, T size)
    {
        return RV_EUNSUPPORTED;
    }

    /**
     * @brief Read memory from bus
     * 
     * @param addr 
     * @param ptr 
     * @param len 
     * @return err_t 
     */
    virtual err_t read(T addr, void* ptr, T len)  = 0;

    /**
     * @brief Write memory to bus
     * 
     * @param addr 
     * @param ptr 
     * @param len
     * @return err_t 
     */
    virtual err_t write(T addr, void* ptr, T len) = 0;

    /**
     * @brief Lock the bus, ths lock must could call recursived
     * 
     */
    virtual void lock() = 0;

    /**
     * @brief Unlock the bus, ths lock must could call recursived
     * 
     */
    virtual void unlock() = 0;
};

/**
 * @brief Instruction interface
 * 
 * @tparam T CPU width type, uint32_t or uint64_t
 */
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

}

#endif /* __ZMMFLY_RVVM_INTF_H__ */
