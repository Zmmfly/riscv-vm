#include "zmmfly/rv32A.h"

namespace zmmfly::rv32
{

using namespace rv;

A::A()
{}

std::string A::name()
{
    return "A";
}

rv_err_t A::execute_normal(uint32_t inst, registers_t& regs, bus_t& bus, inst_map_t& inst_map)
{
    rv_err_t res = RV_EUNSUPPORTED;
    inst_type& iref = reinterpret_cast<inst_type&>(inst);
    regs.x[0] = 0;
    if (iref.opcode != 0b01'011'11 || iref.R_amo.func3 != 0b010) return res;
    switch(iref.R_amo.func5) {
        case 0b000'10:  /* lr.w */ {
            auto addr = regs.x[iref.R_amo.rs1];
            // check align
            if (addr & 0x3) {
                res = RV_EUNALIGNED;
                break;
            }

            uint32_t value = 0;
            if (m_revs.find(addr) == m_revs.end()) {
                uint32_t listener_id = 0;

                /* read and check */
                res = bus.read(addr, &value, 4);
                if (res != RV_EOK) break;

                /* register listener */
                auto fn = std::bind(&A::listener_write, this, 
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
                bus.write_listen(addr, 4, fn, listener_id);

                /* add to revs */
                std::lock_guard lck(m_mtx_revs);
                m_revs[addr] = std::make_tuple(value, false, listener_id);

            } else {
                // auto res = bus.read(addr, &value, 4);
                bus.read(addr, &value, 4);
                std::get<1>(m_revs[addr]) = value;
            }
            regs.x[iref.R_amo.rd] = value;
            break;
        }
        case 0b000'11:  /* sc.w */ {
            auto addr = regs.x[iref.R_amo.rs1];
            if (addr & 0x3) {
                res = RV_EUNALIGNED;
                break;
            }

            uint32_t value     = 0;
            uint32_t listen_id = 0;
            bool     changed   = false;

            std::lock_guard lck(m_mtx_revs);
            if (m_revs.find(addr) == m_revs.end()) {
                regs.x[iref.R_amo.rd] = 1;
                break;
            }
        
            std::tie(value, changed, listen_id) = m_revs[addr];

            /* unlisten */
            bus.write_unlisten(listen_id);

            /* break early if changed */
            if (changed) {
                regs.x[iref.R_amo.rd] = 2;
                break;
            }

            res = bus.write(regs.x[iref.R_amo.rs1], &regs.x[iref.R_amo.rs2], sizeof(regs.x[iref.R_amo.rs2]));
            regs.x[iref.R_amo.rd] = 0;

            break;
        }
        case 0b000'01:  /* amoswap.w */ {
            auto addr = regs.x[iref.R_amo.rs1];
            // check align
            if (addr & 0x3) {
                res = RV_EUNALIGNED;
                break;
            }

            // x[rd] = M[x[rs1]]
            res = bus.read(addr, &regs.x[iref.R_amo.rd], 4);
            if (res != RV_EOK) break;

            res = bus.write(addr, &regs.x[iref.R_amo.rs2], 4);
            if (res != RV_EOK) break;
            break;
        }
        case 0b000'00:  /* amoadd.w */ {
            auto addr = regs.x[iref.R_amo.rs1];
            // check align
            if (addr & 0x3) {
                res = RV_EUNALIGNED;
                break;
            }

            // x[rd] = M[x[rs1]]
            res = bus.read(addr, &regs.x[iref.R_amo.rd], 4);
            if (res != RV_EOK) break;

            uint32_t t_add = regs.x[iref.R_amo.rd] + regs.x[iref.R_amo.rs2];
            res = bus.write(addr, &t_add, sizeof(t_add));
            if (res != RV_EOK) break;
            break;
        }
        case 0b001'00:  /* amoxor.w */ {
            auto addr = regs.x[iref.R_amo.rs1];
            // check align
            if (addr & 0x3) {
                res = RV_EUNALIGNED;
                break;
            }

            // x[rd] = M[x[rs1]]
            res = bus.read(addr, &regs.x[iref.R_amo.rd], 4);
            if (res != RV_EOK) break;

            uint32_t t_xor = regs.x[iref.R_amo.rd] ^ regs.x[iref.R_amo.rs2];
            res = bus.write(addr, &t_xor, sizeof(t_xor));
            if (res != RV_EOK) break;
            break;
        }
        case 0b011'00:  /* amoand.w */ {
            auto addr = regs.x[iref.R_amo.rs1];
            // check align
            if (addr & 0x3) {
                res = RV_EUNALIGNED;
                break;
            }

            // x[rd] = M[x[rs1]]
            res = bus.read(addr, &regs.x[iref.R_amo.rd], 4);
            if (res != RV_EOK) break;

            uint32_t t_and = regs.x[iref.R_amo.rd] & regs.x[iref.R_amo.rs2];
            res = bus.write(addr, &t_and, sizeof(t_and));
            if (res != RV_EOK) break;
            break;
        }
        case 0b010'00:  /* amoor.w */ {
            auto addr = regs.x[iref.R_amo.rs1];
            // check align
            if (addr & 0x3) {
                res = RV_EUNALIGNED;
                break;
            }

            // x[rd] = M[x[rs1]]
            res = bus.read(addr, &regs.x[iref.R_amo.rd], 4);
            if (res != RV_EOK) break;

            uint32_t t_or = regs.x[iref.R_amo.rd] | regs.x[iref.R_amo.rs2];
            res = bus.write(addr, &t_or, sizeof(t_or));
            if (res != RV_EOK) break;
            break;
        }
        case 0b100'00:  /* amomin.w */ {
            auto addr = regs.x[iref.R_amo.rs1];
            // check align
            if (addr & 0x3) {
                res = RV_EUNALIGNED;
                break;
            }

            // x[rd] = M[x[rs1]]
            res = bus.read(addr, &regs.x[iref.R_amo.rd], 4);
            if (res != RV_EOK) break;

            uint32_t t_min = (int32_t)regs.x[iref.R_amo.rd] > (int32_t)regs.x[iref.R_amo.rs2] ? 
                            regs.x[iref.R_amo.rs2] : regs.x[iref.R_amo.rd];
            res = bus.write(addr, &t_min, sizeof(t_min));
            if (res != RV_EOK) break;
            break;
        }
        case 0b101'00:  /* amomax.w */ {
            auto addr = regs.x[iref.R_amo.rs1];
            // check align
            if (addr & 0x3) {
                res = RV_EUNALIGNED;
                break;
            }

            // x[rd] = M[x[rs1]]
            res = bus.read(addr, &regs.x[iref.R_amo.rd], 4);
            if (res != RV_EOK) break;

            uint32_t t_min = (int32_t)regs.x[iref.R_amo.rd] < (int32_t)regs.x[iref.R_amo.rs2] ? 
                            regs.x[iref.R_amo.rs2] : regs.x[iref.R_amo.rd];
            res = bus.write(addr, &t_min, sizeof(t_min));
            if (res != RV_EOK) break;
            break;
        }
        case 0b110'00:  /* amominu.w */ {
            auto addr = regs.x[iref.R_amo.rs1];
            // check align
            if (addr & 0x3) {
                res = RV_EUNALIGNED;
                break;
            }

            // x[rd] = M[x[rs1]]
            res = bus.read(addr, &regs.x[iref.R_amo.rd], 4);
            if (res != RV_EOK) break;

            uint32_t t_minu = regs.x[iref.R_amo.rd] > regs.x[iref.R_amo.rs2] ? 
                            regs.x[iref.R_amo.rs2] : regs.x[iref.R_amo.rd];
            res = bus.write(addr, &t_minu, sizeof(t_minu));
            if (res != RV_EOK) break;
            break;
        }
        case 0b111'00:  /* amomaxu.w */ {
            auto addr = regs.x[iref.R_amo.rs1];
            // check align
            if (addr & 0x3) {
                res = RV_EUNALIGNED;
                break;
            }

            // x[rd] = M[x[rs1]]
            res = bus.read(addr, &regs.x[iref.R_amo.rd], 4);
            if (res != RV_EOK) break;

            uint32_t t_maxu = regs.x[iref.R_amo.rd] < regs.x[iref.R_amo.rs2] ? 
                            regs.x[iref.R_amo.rs2] : regs.x[iref.R_amo.rd];
            res = bus.write(addr, &t_maxu, sizeof(t_maxu));
            if (res != RV_EOK) break;
            break;
        }
        default:
            break;
    }
    regs.x[0] = 0;
    return res;
}

void A::listener_write(uint32_t addr, uint32_t offset, void* ptr, size_t len)
{
    std::lock_guard lck(m_mtx_revs);
    if (m_revs.find(addr) == m_revs.end()) return;

    if (len > sizeof(addr) - offset) return;

    bool changed;
    uint32_t value = 0, newval = 0;

    /* get reserved info */
    std::tie(value, changed, std::ignore) = m_revs[addr];

    /* build new value */
    newval = value;
    memcpy(&newval + offset, ptr, len);

    if (value == newval) return;

    /* update */
    std::get<0>(m_revs[addr]) = newval;
    std::get<1>(m_revs[addr]) = true;
}

}
