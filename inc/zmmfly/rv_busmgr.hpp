#ifndef __ZMMFLY_RV_COMMON_BUSMGR_H__
#define __ZMMFLY_RV_COMMON_BUSMGR_H__

#include "zmmfly/rv_intf.h"
#include <string>
#include <functional>
#include <mutex>
#include <memory>
#include <shared_mutex>

namespace zmmfly::rv
{

template<
    typename T, 
    typename T_mtx        = std::mutex,
    typename T_lock_guard = std::lock_guard<std::mutex>,
    typename T_mtx_shared = std::shared_mutex,
    typename T_lock_rd    = std::shared_lock<std::shared_mutex>,
    typename T_lock_wr    = std::lock_guard<std::shared_mutex>
>
class busmgr
:public bus_intf<T>
{
public:
    using op_arg_t          = std::tuple<std::string, std::any>;
    using bus_intf_ptr_t    = std::shared_ptr<mem_intf<T>>;
    using mem_item_t        = std::tuple<T, size_t, bus_intf_ptr_t>;
    using mem_target_item_t = std::tuple<bus_intf_ptr_t, T, size_t>;   /* buf interface, offset, size */
    using mem_targets_t     = std::vector<mem_target_item_t>;
    using listener_t        = bus_intf<T>::listener_t;
    using listener_info_t   = bus_intf<T>::listener_info_t;

    busmgr(bool is_ibus = false)
    {
        m_is_ibus = is_ibus;
    }

    ~busmgr()
    {}

    rv_err_t mount(T addr, size_t size, bus_intf_ptr_t mem)
    {
        T_lock_guard lck(m_mtx_mems);

        /* check overlap */
        if (has_overlap(addr, size)) return RV_EOVERLAP;

        /* push to vector */
        auto item = mem_item_t(addr, size, mem);
        m_mems.push_back(item);

        /* return if only one */
        if (m_mems.size() == 1) return RV_EOK;

        /* sort for detect optimize */
        std::sort(m_mems.begin(), m_mems.end(), [](mem_item_t& a, mem_item_t& b){
            return std::get<0>(a) < std::get<0>(b);
        });

        return RV_EOK;
    }

    rv_err_t read(T addr, void* ptr, size_t len, std::any arg)
    {
        mem_targets_t mems;
        {
            T_lock_guard lck(m_mtx_mems);
            if (m_is_ibus && ((len == 4 && (addr&0b11)) || (len == 2 && (addr&1))) ) return RV_EUNALIGNED;
            if (!detect_mems(addr, len, mems)) return RV_EBUSFAULT;
        }

        T_lock_rd lck(m_mtx_mem_rw);
        size_t cnt = 0;
        uint8_t* p = (uint8_t*)ptr;
        for (auto mem:mems) {
            auto [intf, offset, len] = mem;
            auto res = intf->read(offset, (void*)(p+cnt), len);
            if (res != RV_EOK) return res;
            cnt += len;
        }

        feed_listeners_rd(addr, ptr, len);

        return RV_EOK;
    }

    rv_err_t read_listen(T addr, size_t len, listener_t fn, T& listen_id)
    {
        if (is_overflow(addr, len)) return RV_EOVERFLOW;
        T_lock_guard lck(m_mtx_listen_rd);

        listen_id = m_listen_rd_count++;
        m_listeners_rd[listen_id] = std::make_tuple(addr, len, fn);
        
        return RV_EOK;
    }

    rv_err_t read_unlisten(T listen_id)
    {
        T_lock_guard lck(m_mtx_listen_rd);

        if (m_listeners_rd.find(listen_id) == m_listeners_rd.end()) return RV_ENOTFOUND;
        m_listeners_rd.erase(listen_id);

        return RV_EOK;
    }

    rv_err_t write(T addr, void* ptr, size_t len, std::any arg)
    {
        mem_targets_t mems;
        {
            T_lock_guard lck(m_mtx_mems);
            if (m_is_ibus && ((len == 4 && (addr&0b11)) || (len == 2 && (addr&1))) ) return RV_EUNALIGNED;
            if (!detect_mems(addr, len, mems)) return RV_EBUSFAULT;
        }

        T_lock_wr lck(m_mtx_mem_rw);
        size_t cnt = 0;
        uint8_t* p = (uint8_t*)ptr;
        for (auto mem:mems) {
            auto [intf, offset, len] = mem;
            auto res = intf->write(offset, (void*)(p+cnt), len);
            if (res != RV_EOK) return res;
            cnt += len;
        }

        feed_listeners_wr(addr, ptr, len);

        return RV_EOK;
    }

    rv_err_t write_listen(T addr, size_t len, listener_t fn, T& listen_id)
    {
        if (is_overflow(addr, len)) return RV_EOVERFLOW;
        T_lock_guard lck(m_mtx_listen_wr);

        listen_id = m_listen_wr_count++;
        m_listeners_wr[listen_id] = listener_info_t(addr, len, fn);
        
        return RV_EOK;
    }

    rv_err_t write_unlisten(T listen_id)
    {
        T_lock_guard lck(m_mtx_listen_wr);

        if (m_listeners_wr.find(listen_id) == m_listeners_wr.end()) return RV_ENOTFOUND;
        m_listeners_wr.erase(listen_id);

        return RV_EOK;
    }

    rv_err_t set(std::string k, std::any v) { 
        return RV_EUNSUPPORTED; 
    }

private:
    void feed_listeners_rd(T mem_addr, void* mem_ptr, size_t mem_len)
    {
        feed_listeners(mem_addr, mem_ptr, mem_len, m_mtx_listen_rd, m_listeners_rd);
    }

    void feed_listeners_wr(T mem_addr, void* mem_ptr, size_t mem_len)
    {
        feed_listeners(mem_addr, mem_ptr, mem_len, m_mtx_listen_wr, m_listeners_wr);
    }

    void feed_listeners(T mem_addr, void* mem_ptr, size_t mem_len, T_mtx& mtx, std::map<T, listener_info_t>& maps)
    {
        T_lock_guard lck(mtx);
        for (auto it:maps) {
            T      listen_addr = std::get<0>(it.second);
            size_t listen_len  = std::get<1>(it.second);
            auto   listen_call = std::get<2>(it.second);
            T listen_off = 0, mem_off = 0;
            size_t ovl_len = 0;

            if (!detect_overlap(listen_addr, listen_len, mem_addr, mem_len, mem_off, listen_off, ovl_len)) continue;

            listen_call(listen_addr, listen_off, mem_ptr, ovl_len);
        }
    }

    bool detect_overlap(T flt_addr, size_t flt_len, T dst_addr, size_t dst_len, 
        T& dst_off, T& flt_off, size_t& overlap_len)
    {
        // dst_addr out of flt range
        if ((dst_addr < flt_addr && dst_addr + dst_len < flt_addr) || dst_addr >= flt_addr + flt_len) 
        {
            return false;
        }
 
        dst_off     = (dst_addr < flt_addr) ? flt_addr - dst_addr : 0;
        flt_off     = (dst_addr < flt_addr) ? 0 : dst_addr - flt_addr;
        overlap_len = (dst_addr < flt_addr) ? dst_addr + dst_len - flt_addr : flt_addr + flt_len - dst_addr;
        return true;
    }

    /**
     * @brief Check address and len range is overflow or not
     * 
     * @param addr 
     * @param len 
     * @return true 
     * @return false 
     */
    bool is_overflow(T addr, size_t len)
    {
        T      addr_curr = addr;
        size_t rm        = len;
        size_t oplen     = 0;
        // size_t cnt       = 0;

        mem_item_t mem;
        do {
            /* find current match */
            if (!detect_mem(addr_curr, mem)) return true;

            /* get mem info */
            T    mem_addr = std::get<0>(mem);
            T    mem_size = std::get<1>(mem);
            auto mem_intf = std::get<2>(mem);

            /* get mem rm and offset */
            T mem_rm     = mem_addr + mem_size - addr_curr;
            T mem_offset = addr_curr - mem_addr;

            oplen = mem_rm > rm ? rm : mem_rm;

            /* increase and decrease */
            rm        -= oplen;
            // cnt       += oplen;
            addr_curr += oplen;
        }while(rm);
        return false;
    }

    /**
     * @brief Detect memorys for address start and size
     * 
     * @param addr 
     * @param size 
     * @param mems 
     * @return true 
     * @return false 
     */
    bool detect_mems(T addr, size_t size, mem_targets_t& mems)
    {
        T      addr_curr = addr;
        size_t rm        = size;
        size_t oplen     = 0;
        // size_t cnt       = 0;

        mem_item_t mem;
        mems.clear();
        do {
            /* find current match */
            if (!detect_mem(addr_curr, mem)) return false;

            /* get mem info */
            T    mem_addr = std::get<0>(mem);
            T    mem_size = std::get<1>(mem);
            auto mem_intf = std::get<2>(mem);

            /* get mem rm and offset */
            T mem_rm     = mem_addr + mem_size - addr_curr;
            T mem_offset = addr_curr - mem_addr;

            oplen = mem_rm > rm ? rm : mem_rm;

            /* push to mems */
            mems.push_back(mem_target_item_t(mem_intf, mem_offset, oplen));

            /* increase and decrease */
            rm        -= oplen;
            // cnt       += oplen;
            addr_curr += oplen;
        }while(rm);
        return true;
    }

    /**
     * @brief Detect memory 
     * 
     * @param addr memory address
     * @param out memory item output
     * @return true 
     * @return false 
     */
    bool detect_mem(T addr, mem_item_t& out)
    {
        for (auto &it:m_mems) {
            T mem_beg = std::get<0>(it);
            T mem_end = std::get<0>(it) + std::get<1>(it);
            if (!(addr >= mem_beg && addr < mem_end)) continue;
            out = it;
            return true;
        }
        return false;
    }

    /**
     * @brief Check new mem has overlap with exists or not
     * 
     * @param addr 
     * @param size 
     * @return true 
     * @return false 
     */
    bool has_overlap(T addr, size_t size)
    {
        T start = addr;
        T end = addr + size;
        for (auto &it:m_mems)
        {
            T mem_addr;
            size_t mem_size;
            std::tie(mem_addr, mem_size, std::ignore) = it;
            if (start >= mem_addr && start < (mem_addr+mem_size)) return true;
            if (end >= mem_addr && end < (mem_addr+mem_size)) return true;
        }
        return false;
    }

private:
    std::vector<mem_item_t> m_mems;
    std::map<T, listener_info_t> m_listeners_rd, m_listeners_wr;
    T_mtx m_mtx_mems;
    T_mtx_shared m_mtx_mem_rw;
    T_mtx m_mtx_listen_rd, m_mtx_listen_wr;
    T    m_listen_rd_count = 0;
    T    m_listen_wr_count = 0;
    bool m_is_ibus         = false;
};

}

#endif /* __ZMMFLY_RV_COMMON_BUSMGR_H__ */
