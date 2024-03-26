#ifndef __ZMMFLY_RVVM_H__
#define __ZMMFLY_RVVM_H__

#include "Zmmfly/rvvm/defs.hpp"
#include "Zmmfly/rvvm/intf.hpp"

namespace Zmmfly::rvvm
{

template<typename T, typename T_recursive_mtx, typename T_lock_guard>
class bus
:public bus_intf<T>
{

private:
    T    m_listen_cnt = 1;
    bool m_is_ibus    = false;
    T_recursive_mtx m_mtx;
    std::map<T, listen_info_t> m_listens_read, m_listens_write;
    std::map<std::tuple<T, T>, std::shared_ptr<mem_intf<T>>> m_mems;

private:
    bool mem_has_overlap(T addr, T size)
    {
        T addr_beg = addr;
        T addr_end = addr + size;
        for (auto it : m_mems) {
            auto [mem_addr, mem_size] = it.first;
            T mem_beg = mem_addr;
            T mem_end = mem_addr + mem_size;

            if (addr_beg < mem_end && addr_end > mem_beg) return true;
        }
        return false;
    }

    /**
     * @brief Get all matched listen calls
     * 
     * @param map 
     * @param addr 
     * @param size 
     * @return std::vector<listen_call_t> 
     */
    std::vector<std::tuple<T, listen_call_t>> match_listen_calls(std::map<T, listen_info_t>& map, T addr, T size) {
        std::vector<listen_call_t> matched_calls;
        T addr_end = addr + size; // 结束地址，注意可能的溢出

        for (auto& [key, value] : map) {
            auto [listen_addr, listen_size, listen_call] = value;

            T listen_end = listen_addr + listen_size;

            if (addr < listen_end && addr_end > listen_addr) {
                matched_calls.push_back(std::make_tuple(key, listen_call));
            }
        }

        return matched_calls;
    }

    bool call_read_listen(T addr, void* ptr, T size, bool before_op) {
        auto matched_calls = match_listen_calls(m_listens_read, addr, size);
        bool last_hooked = false;
        bool hooked = false;
        T last_listen_id = 0;
        for (auto& info : matched_calls) {
            auto [listen_id, call] = info;
            last_hooked = call(addr, ptr, size, last_listen_id, last_hooked, before_op);
            last_listen_id = listen_id;
            if (last_hooked) hooked = true;
        }
        return hooked
    }

    bool call_write_listen(T addr, void* ptr, T size, bool before_op) {
        auto matched_calls = match_listen_calls(m_listens_write, addr, size);
        bool last_hooked = false;
        bool hooked = false;
        T last_listen_id = 0;
        for (auto& info : matched_calls) {
            auto [listen_id, call] = info;
            last_hooked = call(addr, ptr, size, last_listen_id, last_hooked, before_op);
            last_listen_id = listen_id;
            if (last_hooked) hooked = true;
        }
        return hooked
    }

public:
    bus(bool is_ibus = false)
    {
        m_is_ibus = is_ibus;
    }

    err_t mount(T addr, T size, std::shared_ptr<mem_intf<T>> mem)
    {
        T_lock_guard lck(m_mtx);
        if (mem_has_overlap(addr, size)) return RV_EOVERLAP;

        m_mems[std::make_tuple(addr, size)] = mem;

        return RV_EOK;
    }

    err_t unmount(T addr, T size)
    {
        T_lock_guard lck(m_mtx);
        auto key = std::make_tuple(addr, size);
        if (m_mems.find(key) == m_mems.end()) return RV_ENOTFOUND;
        m_mems.erase(key);
        return RV_EOK;
    }

    err_t unmount(std::shared_ptr<mem_intf<T>> mem)
    {
        T_lock_guard lck(m_mtx);
        for (auto it:m_mems) {
            if (it.second == mem) {
                m_mems.erase(it.first);
                return RV_EOK;
            }
        }
        return RV_ENOTFOUND;
    }

    err_t read(T addr, void* ptr, T len)
    {
        if (m_is_ibus && (addr & 1)) return RV_EUNALIGNED;

        T_lock_guard lck(m_mtx);
        bool hooked  = false;
        T    mem_beg = 0,    mem_end = 0;
        T    add_beg = addr, add_end = addr+len;
        std::shared_ptr<mem_intf<T>> mem;

        hooked = call_read_listen(addr, ptr, len, true);
        if (hooked) return RV_EOK;

        for (auto it:m_mems) {
            auto [mem_addr, mem_size] = it.first;
            mem_beg = mem_addr;
            mem_end = mem_addr + mem_size;

            if ( !(add_beg >= mem_beg && add_end <= mem_beg) ) continue;
            auto res = it.second->read(add_beg - mem_beg, ptr, len);
            call_read_listen(addr, ptr, len, false);
            return res;
        }
        return RV_ENOTFOUND;
    }

    err_t write(T addr, void* ptr, T len)
    {
        T_lock_guard lck(m_mtx);
        bool hooked = false;
        T mem_beg = 0,    mem_end = 0;
        T add_beg = addr, add_end = addr+len;
        std::shared_ptr<mem_intf<T>> mem;

        hooked = call_write_listen(addr, ptr, len, true);
        if (hooked) return RV_EOK;

        for (auto it:m_mems) {
            auto [mem_addr, mem_size] = it.first;
            mem_beg = mem_addr;
            mem_end = mem_addr + mem_size;

            if ( !(add_beg >= mem_beg && add_end <= mem_beg) ) continue;
            auto res = it.second->write(add_beg - mem_beg, ptr, len);
            call_write_listen(addr, ptr, len, false);
            return res;
        }
        return RV_ENOTFOUND;
    }

    void lock()
    {
        m_mtx.lock();
    }

    void unlock()
    {
        m_mtx.unlock();
    }

    /**
     * @brief Register read listen
     * 
     * @param addr 
     * @param size 
     * @param call 
     * @param listen_id listen id out
     * @return err_t 
     */
    err_t listen_read(T addr, T size, listen_call_t call, T& listen_id) {
        T_lock_guard lock(m_mtx);
        listen_id = m_listen_cnt++;
        m_listens_read[listen_id] = std::make_tuple(addr, size, call);
        return RV_EOK;
    }

    /**
     * @brief Register write listen
     * 
     * @param addr 
     * @param size 
     * @param call 
     * @param listen_id listen id out
     * @return err_t 
     */
    err_t listen_write(T addr, T size, listen_call_t call, T& listen_id) {
        T_lock_guard lock(m_mtx);
        listen_id = m_listen_cnt++;
        m_listens_write[listen_id] = std::make_tuple(addr, size, call);
        return RV_EOK;
    }

    /**
     * @brief Unlisten read with listen id
     * 
     * @param listen_id 
     * @return err_t 
     */
    err_t unlisten_read(T listen_id) {
        T_lock_guard lock(m_mtx);
        if (m_listens_read.erase(listen_id)) {
            return RV_EOK;
        }
        return RV_ENOTFOUND;
    }

    /**
     * @brief Unlisten read with addr and size
     * 
     * @param addr 
     * @param size 
     * @return err_t 
     */
    err_t unlisten_read(T addr, T size) {
        bool found = false;
        T_lock_guard lock(m_mtx);
        for (auto it = m_listens_read.begin(); it != m_listens_read.end(); ) {
            if (std::get<0>(it->second) == addr && std::get<1>(it->second) == size) {
                it = m_listens_read.erase(it);
                found = true;
            } else {
                ++it;
            }
        }
        return found ? RV_EOK : RV_ENOTFOUND;
    }

    err_t unlisten_write(T listen_id) {
        T_lock_guard lock(m_mtx);
        if (m_listens_write.erase(listen_id)) {
            return RV_EOK;
        }
        return RV_ENOTFOUND;
    }

    err_t unlisten_write(T addr, T size) {
        bool found = false;
        T_lock_guard lock(m_mtx);
        for (auto it = m_listens_write.begin(); it != m_listens_write.end(); ) {
            if (std::get<0>(it->second) == addr && std::get<1>(it->second) == size) {
                it = m_listens_write.erase(it);
                found = true;
            } else {
                ++it;
            }
        }
        return found ? RV_EOK : RV_ENOTFOUND;
    }
};

}

#endif /* __ZMMFLY_RVVM_H__ */
