#ifndef __ZMMFLY_RV_COMMON_BUSMGR_H__
#define __ZMMFLY_RV_COMMON_BUSMGR_H__

#include "zmmfly/rv_intf.h"

namespace zmmfly::rv
{

template<typename T, typename T_mtx, typename T_lock_guard>
class busmgr
:public bus_mgr_intf<T>
{
public:
    using bus_intf_ptr_t    = std::shared_ptr<bus_intf<T>>;
    using mem_item_t        = std::tuple<T, size_t, bus_intf_ptr_t>;
    using mem_target_item_t = std::tuple<bus_intf_ptr_t, T, size_t>;   /* buf interface, offset, size */
    using mem_targets_t     = std::vector<mem_target_item_t>;
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

    rv_err_t read(T addr, void* ptr, size_t len)
    {
        T_lock_guard lck(m_mtx_mems);
        mem_targets_t mems;
        if (m_is_ibus && addr&1) return RV_EUNALIGNED;
        if (!detect_mems(addr, len, mems)) return RV_EBUSFAULT;

        size_t cnt = 0;
        for (auto mem:mems) {
            uint8_t* p = (uint8_t*)ptr;
            auto [intf, offset, len] = mem;
            auto res = intf->read(offset, (void*)p, len);
            if (res != RV_EOK) return res;
            cnt += len;
            p   += len;
        }
        return RV_EOK;
    }

    rv_err_t write(T addr, void* ptr, size_t len)
    {
        T_lock_guard lck(m_mtx_mems);
        mem_targets_t mems;
        if (m_is_ibus) return RV_EBUSFAULT;
        if (!detect_mems(addr, len, mems)) return RV_EBUSFAULT;

        size_t cnt = 0;
        for (auto mem:mems) {
            uint8_t* p = (uint8_t*)ptr;
            auto [intf, offset, len] = mem;
            auto res = intf->write(offset, (void*)p, len);
            if (res != RV_EOK) return res;
            cnt += len;
            p   += len;
        }
        return RV_EOK;
    }

private:
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
        size_t cnt       = 0;

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
            cnt       += oplen;
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
    T_mtx m_mtx_mems;
    bool m_is_ibus = false;
};

}

#endif /* __ZMMFLY_RV_COMMON_BUSMGR_H__ */
