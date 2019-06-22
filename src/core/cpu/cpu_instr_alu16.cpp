#include "cpu_instr.hpp"

#include "cpu/cpu.hpp"

namespace LibDMG
{
    /**************************************************************************************************/
    /* 16-bit ALU                                                                                     */
    /**************************************************************************************************/

    //..................................................................................................
    void CpuInstr::incReg16(Cpu& cpu, Cpu::Reg16 reg)
    {        
	    cpu.m_instrCycles = 8;
	    cpu.setReg16(reg, cpu.reg16(reg) + 1);
    }

    //..................................................................................................
    void CpuInstr::decReg16(Cpu& cpu, Cpu::Reg16 reg)
    {
        cpu.m_instrCycles = 8;
        cpu.setReg16(reg, cpu.reg16(reg) - 1);
    }

    //..................................................................................................
    void CpuInstr::addHlReg16(Cpu& cpu, Cpu::Reg16 reg)
    {
        cpu.m_instrCycles = 8;

        uint16_t hlVal = cpu.reg16(Cpu::REG16_HL);
        uint16_t argVal = cpu.reg16(reg);
        cpu.setReg16(Cpu::REG16_HL, hlVal + argVal);
        cpu.setFlagN(false);
        // Flags H & C TODO
    }

    //..................................................................................................
    void CpuInstr::addSpImm(Cpu& cpu, MemControllerBase& mem)
    {
        cpu.m_instrCycles = 16;
        fetchParam8(cpu, mem);

        uint16_t spVal = cpu.reg16(Cpu::REG16_SP);
        uint16_t immVal = cpu.m_parameters[0];
        cpu.setReg16(Cpu::REG16_SP, spVal + immVal);
        cpu.setFlagZ(false);
        cpu.setFlagN(false);
        // Flags H & C TODO

    }
}