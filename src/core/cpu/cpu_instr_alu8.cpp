#include "cpu_instr.hpp"

#include "cpu/cpu.hpp"
#include "cpu/cpu_macros.hpp"
#include "mem/mem_controller_base.hpp"

namespace LibDMG
{
    /**************************************************************************************************/
    /* 8-bit ALU                                                                                      */
    /**************************************************************************************************/

    //..................................................................................................
    void CpuInstr::addReg8(Cpu& cpu, Cpu::Reg8 reg, bool carry = false)
    {
        cpu.m_instrCycles = 4;

        uint8_t aVal = cpu.reg8(Cpu::REG8_A);
        uint8_t regVal = cpu.reg8(reg);
        if (carry)
        {
            uint8_t carryVal = FLAG_TO_UINT(cpu.flagC());
            cpu.setReg8(Cpu::REG8_A, cpu.reg8(Cpu::REG8_A) + regVal + carryVal);
            cpu.setFlagH(IS_HALF_CARRY3(aVal, regVal, carryVal));
            cpu.setFlagC(IS_CARRY3(aVal, regVal, carryVal));
        }
        else
        {
            cpu.setReg8(Cpu::REG8_A, cpu.reg8(Cpu::REG8_A) + regVal);
            cpu.setFlagH(IS_HALF_CARRY2(aVal, regVal));
            cpu.setFlagC(IS_CARRY2(aVal, regVal));
        }
        cpu.setFlagZ(IS_ZERO(cpu.reg8(Cpu::REG8_A)));
        cpu.setFlagN(false);
    }

    //..................................................................................................
    void CpuInstr::addMem(Cpu& cpu, MemControllerBase& mem, Cpu::Reg16 reg, bool carry = false)
    {
        cpu.m_instrCycles = 8;

        uint8_t aVal = cpu.reg8(Cpu::REG8_A);
        uint8_t memVal = mem.read(cpu.reg16(reg));
        if (carry)
        {
            uint8_t carryVal = FLAG_TO_UINT(cpu.flagC());
            cpu.setReg8(Cpu::REG8_A, cpu.reg8(Cpu::REG8_A) + memVal + carryVal);
            cpu.setFlagH(IS_HALF_CARRY3(aVal, memVal, carryVal));
            cpu.setFlagC(IS_CARRY3(aVal, memVal, carryVal));
        }
        else
        {
            cpu.setReg8(Cpu::REG8_A, cpu.reg8(Cpu::REG8_A) + memVal);
            cpu.setFlagH(IS_HALF_CARRY2(aVal, memVal));
            cpu.setFlagC(IS_CARRY2(aVal, memVal));
        }
        cpu.setFlagZ(IS_ZERO(cpu.reg8(Cpu::REG8_A)));
        cpu.setFlagN(false);
    }

    //..................................................................................................
    void CpuInstr::addImm(Cpu& cpu, MemControllerBase& mem, bool carry = false)
    {

        cpu.m_instrCycles = 8;
        fetchParam8(cpu, mem);

        uint8_t aVal = cpu.reg8(Cpu::REG8_A);
        uint8_t immVal = cpu.m_parameters[0];
        if (carry)
        {
            uint8_t carryVal = FLAG_TO_UINT(cpu.flagC());
            cpu.setReg8(Cpu::REG8_A, cpu.reg8(Cpu::REG8_A) + immVal + carryVal);
            cpu.setFlagH(IS_HALF_CARRY3(aVal, immVal, carryVal));
            cpu.setFlagC(IS_CARRY3(aVal, immVal, carryVal));
        }
        else
        {
            cpu.setReg8(Cpu::REG8_A, cpu.reg8(Cpu::REG8_A) + immVal);
            cpu.setFlagH(IS_HALF_CARRY2(aVal, immVal));
            cpu.setFlagC(IS_CARRY2(aVal, immVal));
        }
        cpu.setFlagZ(IS_ZERO(cpu.reg8(Cpu::REG8_A)));
        cpu.setFlagN(false);
    }

    //..................................................................................................
    void CpuInstr::subReg8(Cpu& cpu, Cpu::Reg8 reg, bool carry = false)
    {
        cpu.m_instrCycles = 4;

        uint8_t aVal = cpu.reg8(Cpu::REG8_A);
        uint8_t regVal = cpu.reg8(reg);
        if (carry)
        {
            uint8_t carryVal = FLAG_TO_UINT(cpu.flagC());
            cpu.setReg8(Cpu::REG8_A, cpu.reg8(Cpu::REG8_A) - regVal - carryVal);
            cpu.setFlagH(IS_HALF_BORROW3(aVal, regVal, carryVal));
            cpu.setFlagC(IS_BORROW3(aVal, regVal, carryVal));
        }
        else
        {
            cpu.setReg8(Cpu::REG8_A, cpu.reg8(Cpu::REG8_A) - regVal);
            cpu.setFlagH(IS_HALF_BORROW2(aVal, regVal));
            cpu.setFlagC(IS_BORROW2(aVal, regVal));
        }
        cpu.setFlagZ(IS_ZERO(cpu.reg8(Cpu::REG8_A)));
        cpu.setFlagN(true);
    }

    //..................................................................................................
    void CpuInstr::subMem(Cpu& cpu, MemControllerBase& mem, Cpu::Reg16 reg, bool carry = false)
    {
        cpu.m_instrCycles = 8;

        uint8_t aVal = cpu.reg8(Cpu::REG8_A);
        uint8_t memVal = mem.read(cpu.reg16(reg));
        if (carry)
        {
            uint8_t carryVal = FLAG_TO_UINT(cpu.flagC());
            cpu.setReg8(Cpu::REG8_A, cpu.reg8(Cpu::REG8_A) - memVal - carryVal);
            cpu.setFlagH(IS_HALF_BORROW3(aVal, memVal, carryVal));
            cpu.setFlagC(IS_BORROW3(aVal, memVal, carryVal));
        }
        else
        {
            cpu.setReg8(Cpu::REG8_A, cpu.reg8(Cpu::REG8_A) - memVal);
            cpu.setFlagH(IS_HALF_BORROW2(aVal, memVal));
            cpu.setFlagC(IS_BORROW2(aVal, memVal));
        }
        cpu.setFlagZ(IS_ZERO(cpu.reg8(Cpu::REG8_A)));
        cpu.setFlagN(true);
    }

    //..................................................................................................
    void CpuInstr::subImm(Cpu& cpu, MemControllerBase& mem, bool carry = false)
    {
        cpu.m_instrCycles = 8;
        fetchParam8(cpu, mem);

        uint8_t aVal = cpu.reg8(Cpu::REG8_A);
        uint8_t immVal = cpu.m_parameters[0];
        if (carry)
        {
            uint8_t carryVal = FLAG_TO_UINT(cpu.flagC());
            cpu.setReg8(Cpu::REG8_A, cpu.reg8(Cpu::REG8_A) - immVal - carryVal);
            cpu.setFlagH(IS_HALF_BORROW3(aVal, immVal, carryVal));
            cpu.setFlagC(IS_BORROW3(aVal, immVal, carryVal));
        }
        else
        {
            cpu.setReg8(Cpu::REG8_A, cpu.reg8(Cpu::REG8_A) - immVal);
            cpu.setFlagH(IS_HALF_BORROW2(aVal, immVal));
            cpu.setFlagC(IS_BORROW2(aVal, immVal));
        }
        cpu.setFlagZ(IS_ZERO(cpu.reg8(Cpu::REG8_A)));
        cpu.setFlagN(true);
    }
}