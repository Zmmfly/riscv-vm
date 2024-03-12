#ifndef __ZMMFLY_RVVM_A_H__
#define __ZMMFLY_RVVM_A_H__

#include "Zmmfly/rvvm/defs.hpp"
#include "Zmmfly/rvvm/intf.hpp"
#include "Zmmfly/rvvm/utils.hpp"

namespace Zmmfly::rvvm
{

template<typename T, typename T_mtx, typename T_guard>
class A
:public inst_intf<T>
{
public:
    static_assert(std::is_same<T, uint32_t>::value || std::is_same<T, uint64_t>, 
                "T must be uint32_t or uint64_t");

    using T_sign = typename std::make_signed<T>::type;

    A(){};
    std::string name() { return "A"; }

    err_t execute_normal(uint32_t inst, registers_t& regs, bus_intf_t& bus, inst_map_t& inst_map)
    {
        err_t res = RV_EUNSUPPORTED;
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

                T value = 0;
                if (m_revs.find(addr) == m_revs.end()) {
                    T listener_id = 0;

                    /* read and check */
                    res = bus.read(addr, &value, sizeof(T));
                    if (res != RV_EOK) break;

                    /* register listener */
                    auto fn = std::bind(&A::listen_callback_write, this, 
                        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
                    bus.write_listen(addr, sizeof(T), fn, listener_id);

                    /* add to revs */
                    T_guard lck(m_mtx_revs);
                    m_revs[addr] = std::make_tuple(value, false, listener_id);

                } else {
                    // auto res = bus.read(addr, &value, 4);
                    bus.read(addr, &value, sizeof(T));
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

                T    value     = 0;
                T    listen_id = 0;
                bool changed   = false;

                T_guard lck(m_mtx_revs);
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
                res = bus.read(addr, &regs.x[iref.R_amo.rd], sizeof(T));
                if (res != RV_EOK) break;

                res = bus.write(addr, &regs.x[iref.R_amo.rs2], sizeof(T));
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
                res = bus.read(addr, &regs.x[iref.R_amo.rd], sizeof(T));
                if (res != RV_EOK) break;

                T t_add = regs.x[iref.R_amo.rd] + regs.x[iref.R_amo.rs2];
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
                res = bus.read(addr, &regs.x[iref.R_amo.rd], sizeof(T));
                if (res != RV_EOK) break;

                T t_xor = regs.x[iref.R_amo.rd] ^ regs.x[iref.R_amo.rs2];
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
                res = bus.read(addr, &regs.x[iref.R_amo.rd], sizeof(T));
                if (res != RV_EOK) break;

                T t_and = regs.x[iref.R_amo.rd] & regs.x[iref.R_amo.rs2];
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
                res = bus.read(addr, &regs.x[iref.R_amo.rd], sizeof(T));
                if (res != RV_EOK) break;

                T t_or = regs.x[iref.R_amo.rd] | regs.x[iref.R_amo.rs2];
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
                res = bus.read(addr, &regs.x[iref.R_amo.rd], sizeof(T));
                if (res != RV_EOK) break;

                T t_min = (T_sign)regs.x[iref.R_amo.rd] > (T_sign)regs.x[iref.R_amo.rs2] ? 
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

                T t_min = (T_sign)regs.x[iref.R_amo.rd] < (T_sign)regs.x[iref.R_amo.rs2] ? 
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

                T t_minu = regs.x[iref.R_amo.rd] > regs.x[iref.R_amo.rs2] ? 
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

                T t_maxu = regs.x[iref.R_amo.rd] < regs.x[iref.R_amo.rs2] ? 
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

private:
    void listen_callback_write(T addr, T offset, void* ptr, T len)
    {
        T_guard lck(m_mtx_revs);
        if (m_revs.find(addr) == m_revs.end()) return;

        if (len > sizeof(addr) - offset) return;

        bool changed;
        T value = 0, newval = 0;

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

private:
    T_mtx m_mtx_revs;
    // key: addr, value: tuple<reserved value, changed, listen_id>
    std::map<T, std::tuple<T, bool, T>> m_revs;
};

}

#endif /* __ZMMFLY_RVVM_A_H__ */
