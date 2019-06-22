#include "cpu_instr.hpp"

#include "cpu_macros.hpp"

namespace LibDMG
{
    /**************************************************************************************************/
    /* CB-extended instructions                                                                       */
    /**************************************************************************************************/

    //..................................................................................................
    void CpuInstr::cbBitReg8(Cpu& cpu, Cpu::Reg8 reg, uint8_t index)
    {
        uint8_t mask = 1 << index;
        if ((cpu.reg8(reg) & mask) == mask)
        {
            cpu.setFlagZ(false);
        }
        else
        {
            cpu.setFlagZ(true);
        }
        cpu.setFlagN(false);
        cpu.setFlagH(true);
        cpu.m_instrCycles = 8;
    }

    //..................................................................................................
    void CpuInstr::cbBitMem8(Cpu& cpu, MemControllerBase& mem, uint8_t index)
    {
        uint8_t mask = 1 << index;
        if ((mem.read(cpu.reg16(Cpu::REG16_HL)) & mask) == mask)
        {
            cpu.setFlagZ(false);
        }
        else
        {
            cpu.setFlagZ(true);
        }
        cpu.setFlagN(false);
        cpu.setFlagH(true);
        cpu.m_instrCycles = 16;
    }

    //..................................................................................................
    void CpuInstr::cbResReg8(Cpu& cpu, Cpu::Reg8 reg, uint8_t index)
    {
	    uint8_t mask = !(1 << index);
	    cpu.setReg8(reg, cpu.reg8(reg) & mask);
        cpu.m_instrCycles = 8;
    }

    //..................................................................................................
    void CpuInstr::cbResMem8(Cpu& cpu, MemControllerBase& mem, uint8_t index)
    {
        uint8_t mask = !(1 << index);
        uint8_t val = mem.read(cpu.reg16(Cpu::REG16_HL));
        val &= mask;
        mem.write(cpu.reg16(Cpu::REG16_HL), val);
        cpu.m_instrCycles = 16;
    }

    //..................................................................................................
    void CpuInstr::cbSetReg8(Cpu& cpu, Cpu::Reg8 reg, uint8_t index)
    {
        uint8_t mask = 1 << index;
        cpu.setReg8(reg, cpu.reg8(reg) | mask);
        cpu.m_instrCycles = 8;
    }

    //..................................................................................................
    void CpuInstr::cbSetMem8(Cpu& cpu, MemControllerBase& mem, uint8_t index)
    {
        uint8_t mask = 1 << index;
        uint8_t val = mem.read(cpu.reg16(Cpu::REG16_HL));
        val |= mask;
        mem.write(cpu.reg16(Cpu::REG16_HL), val);
        cpu.m_instrCycles = 16;
    }

    //..................................................................................................
    void CpuInstr::cbRlReg8(Cpu& cpu, Cpu::Reg8 reg, bool rlc)
    {
        cpu.m_instrCycles = 8;
        helperRl(cpu, reg, rlc);
    }

    //..................................................................................................
    void CpuInstr::cbRlMem(Cpu& cpu, MemControllerBase& mem, bool rlc)
    {
        cpu.m_instrCycles = 16;

        uint8_t memVal = mem.read(cpu.reg16(Cpu::REG16_HL));
        uint8_t oldCarryVal = FLAG_TO_UINT(cpu.flagC());
        if ((memVal & 0x80) == 0x80)
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
        uint8_t newVal = (memVal << 1) + arg;
        mem.write(cpu.reg16(Cpu::REG16_HL), newVal);

        cpu.setFlagZ(IS_ZERO(newVal));
        cpu.setFlagN(false);
        cpu.setFlagH(false);
    }

    //..................................................................................................
    void CpuInstr::cbRrReg8(Cpu& cpu, Cpu::Reg8 reg, bool rrc)
    {
        cpu.m_instrCycles = 8;
        helperRr(cpu, reg, rrc);
    }

    //..................................................................................................
    void CpuInstr::cbRrMem(Cpu& cpu, MemControllerBase& mem, bool rrc)
    {
        cpu.m_instrCycles = 16;

        uint8_t memVal = mem.read(cpu.reg16(Cpu::REG16_HL));
        uint8_t oldCarryVal = FLAG_TO_UINT(cpu.flagC());
        if ((memVal & 0x01) == 0x01)
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
        uint8_t newVal = (memVal >> 1) + (arg << 7);
        mem.write(cpu.reg16(Cpu::REG16_HL), newVal);

        cpu.setFlagZ(IS_ZERO(newVal));
        cpu.setFlagN(false);
        cpu.setFlagH(false);
    }
}