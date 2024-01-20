#ifndef __TESTER_RV32I_H__
#define __TESTER_RV32I_H__

#include "zmmfly/rv32.h"
#include <memory>
#include <map>
#include <string>
#include <mutex>

using namespace zmmfly;

class mem
:public rv32::bus_intf_t
{
public:
    mem(size_t mem_size = 4 * 1024);

    rv_err_t read(uint32_t offset, void* ptr, size_t len);
    rv_err_t write(uint32_t offset, void* ptr, size_t len);

private:
    std::vector<uint8_t> m_mem;
};

class tester_rv32i
{
public:
    using inst_ptr_t = std::shared_ptr<rv32::inst_intf_t>;

    rv::busmgr<uint32_t, std::mutex, std::lock_guard<std::mutex>> busmgr;
    rv32::inst_intf_t::registers_t regs;
    std::map<std::string, inst_ptr_t> instmap;

    tester_rv32i();
    ~tester_rv32i();

    void reset();
    void execute_inst(uint32_t inst);
private:
    std::shared_ptr<mem> m_mem;
};

#endif /* __TESTER_RV32I_H__ */
