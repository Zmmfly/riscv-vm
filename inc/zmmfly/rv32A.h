#ifndef __ZMMFLY_RV32_A_H__
#define __ZMMFLY_RV32_A_H__

#include "zmmfly/rv32_base.h"
#include <mutex>

namespace zmmfly::rv32::v1
{

class A
: public inst_intf_t
{

public:
    A();

    std::string name();

    rv_err_t execute_normal(uint32_t inst, registers_t& regs, bus_t& bus, inst_map_t& inst_map);

private:
    void listener_write(uint32_t addr, uint32_t offset, void* ptr, size_t len);

private:
    std::mutex m_mtx_revs;
    // key: addr, value: reserved value, changed, listener_id
    std::map<uint32_t, std::tuple<uint32_t, bool, uint32_t>> m_revs;
};

};

#endif /* __ZMMFLY_RV32_A_H__ */
