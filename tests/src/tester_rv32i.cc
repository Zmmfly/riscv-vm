#include "tester_rv32i.h"

mem::mem(size_t mem_size)
{
    m_mem.resize(mem_size);
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
    instmap["I"] = std::make_shared<rv32::v1::I>();
}

tester_rv32i::~tester_rv32i()
{}

void tester_rv32i::reset()
{}

void tester_rv32i::execute_inst(uint32_t inst)
{}
