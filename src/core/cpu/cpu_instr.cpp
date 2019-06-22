#include "cpu_instr.hpp"

#include "cpu.hpp"
#include "cpu_macros.hpp"
#include "mem/mem_controller_base.hpp"
#include "logger.hpp"

namespace LibDMG
{
    //..................................................................................................
    void CpuInstr::nop(Cpu& cpu)
    {
        cpu.m_instrCycles = 4;
    }

    //..................................................................................................
    void CpuInstr::cb(Cpu& cpu, MemControllerBase& mem)
    {
        fetchParam8(cpu, mem);

        uint8_t reg = cpu.m_parameters[0] & 0x07;
        uint8_t bitCode = (cpu.m_parameters[0] & 0x38) >> 3;
        uint8_t subCode = (cpu.m_parameters[0] & 0xC0) >> 6;

        if (subCode == 0) {
            switch (bitCode) {
            // RLC
            case 0:
                if (reg != 6) {
                    cbRlReg8(cpu, static_cast<Cpu::Reg8>(reg), true);
                }
                else {
                    cbRlMem(cpu, mem, true);
                }
                break;

            // RRC
            case 1:
                if (reg != 6) {
                    cbRrReg8(cpu, static_cast<Cpu::Reg8>(reg), true);
                }
                else {
                    cbRrMem(cpu, mem, true);
                }
                break;

            // RL
            case 2:
                if (reg != 6) {
                    cbRlReg8(cpu, static_cast<Cpu::Reg8>(reg));
                }
                else {
                    cbRlMem(cpu, mem);
                }
                break;

            // RR
            case 3:
                if (reg != 6) {
                    cbRrReg8(cpu, static_cast<Cpu::Reg8>(reg));
                }
                else {
                    cbRrMem(cpu, mem);
                }
                break;

            default:
                LOG_WARN("CPU: Unknown CB extended instruction");
            }
        }
        // BIT
        else if (subCode == 1) {
            if (reg != 6) {
                cbBitReg8(cpu, static_cast<Cpu::Reg8>(reg), bitCode);
            }
            else {
                cbBitMem8(cpu, mem, bitCode);
            }
        }
        // RES
        else if (subCode == 2) {
            if (reg != 6) {
                cbResReg8(cpu, static_cast<Cpu::Reg8>(reg), bitCode);
            }
            else {
                cbResMem8(cpu, mem, bitCode);
            }
        }
        // SET
        else if (subCode == 3) {
            if (reg != 6) {
                cbSetReg8(cpu, static_cast<Cpu::Reg8>(reg), bitCode);
            }
            else {
                cbSetMem8(cpu, mem, bitCode);
            }
        }
        else {
            LOG_WARN("CPU: Internal error");
            // TODO
        }
    }

    //..................................................................................................
    void CpuInstr::helperRl(Cpu& cpu, Cpu::Reg8 reg, bool rlc)
    {
        uint8_t regVal = cpu.reg8(reg);
        uint8_t oldCarryVal = FLAG_TO_UINT(cpu.flagC());
        if ((regVal & 0x80) == 0x80)
        {
            cpu.setFlagC(true);
        }
        else
        {
            cpu.setFlagC(false);
        }
        uint8_t arg;
        if (rlc)
        {
            // Get the new flag
            arg = FLAG_TO_UINT(cpu.flagC());
        }
        else
        {
            arg = oldCarryVal;
        }
        cpu.setReg8(reg, (regVal << 1) + arg);

        cpu.setFlagZ(IS_ZERO(cpu.reg8(reg)));
        cpu.setFlagN(false);
        cpu.setFlagH(false);
    }

    //..................................................................................................
    void CpuInstr::helperRr(Cpu& cpu, Cpu::Reg8 reg, bool rrc)
    {
        uint8_t regVal = cpu.reg8(reg);
        uint8_t oldCarryVal = FLAG_TO_UINT(cpu.flagC());
        if ((regVal & 0x01) == 0x01)
        {
            cpu.setFlagC(true);
        }
        else
        {
            cpu.setFlagC(false);
        }
        uint8_t arg;
        if (rrc)
        {
            // Get the new flag
            arg = FLAG_TO_UINT(cpu.flagC());
        }
        else
        {
            arg = oldCarryVal;
        }
        cpu.setReg8(reg, (regVal >> 1) + (arg << 7));

        cpu.setFlagZ(IS_ZERO(cpu.reg8(reg)));
        cpu.setFlagN(false);
        cpu.setFlagH(false);
    }
}