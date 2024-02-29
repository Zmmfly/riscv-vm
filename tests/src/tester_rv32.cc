#include "tester_rv32i.h"
#include <cstring>

mem::mem(size_t mem_size)
{
    m_mem.resize(mem_size);
}

void mem::reset()
{
    memset(m_mem.data(), 0, m_mem.size());
}

rv_err_t mem::read(uint32_t offset, void* ptr, size_t len)
{
    if (offset >= m_mem.size() || offset + len >= m_mem.size())
    {
        return RV_EIO;
    }
    memcpy(ptr, m_mem.data() + offset, len);
    return RV_EOK;
}

rv_err_t mem::write(uint32_t offset, void* ptr, size_t len)
{
    if (offset >= m_mem.size() || offset + len >= m_mem.size())
    {
        return RV_EIO;
    }
    memcpy(m_mem.data() + offset, ptr, len);
    return RV_EOK;
}

tester_rv32i::tester_rv32i()
{
    instmap["I"] = std::make_shared<rv32::I>();
    instmap["M"] = std::make_shared<rv32::M>();
    m_mem        = std::make_shared<mem>();
    busmgr.mount(0x10000000, m_mem->m_mem.size(), m_mem);
}

tester_rv32i::~tester_rv32i()
{}

void tester_rv32i::reset()
{
    memset(&regs, 0, sizeof(regs));
}

rv_err_t tester_rv32i::exec(uint32_t inst)
{
    for (auto it:instmap) {
        auto ret = it.second->execute_normal(inst, regs, busmgr, instmap);
        if (ret != RV_EUNSUPPORTED) return ret;
    }
    return RV_EUNSUPPORTED;
}
