#include "cpu_instr.hpp"

#include "cpu/cpu.hpp"
#include "cpu/cpu_macros.hpp"
#include "mem/mem_controller_base.hpp"

namespace LibDMG
{
    /**************************************************************************************************/
    /* 16-bit loads                                                                                   */
    /**************************************************************************************************/

    //..................................................................................................
    void CpuInstr::ldReg16Imm(Cpu& cpu, Cpu::Reg16 reg, MemControllerBase& mem)
    {
        cpu.m_instrCycles = 12;
        fetchParam16(cpu, mem);
        cpu.setReg16(reg, (cpu.m_parameters[1] << 8) + cpu.m_parameters[0]);
    }

    //..................................................................................................
    void CpuInstr::ldSpHl(Cpu& cpu)
    {
        cpu.m_instrCycles = 8;
        cpu.setReg16(Cpu::REG16_SP, cpu.reg16(Cpu::REG16_HL));
    }

    //..................................................................................................
    void CpuInstr::ldHlSpImm(Cpu& cpu, MemControllerBase& mem)
    {
        cpu.m_instrCycles = 12;
        fetchParam8(cpu, mem);
        uint16_t result = (uint16_t)((int16_t)cpu.reg16(Cpu::REG16_SP) + (int16_t)cpu.m_parameters[0]);
        cpu.setReg16(Cpu::REG16_HL, result);

        cpu.setFlagZ(false);
        cpu.setFlagN(false);
        cpu.setFlagH(IS_HALF_CARRY2(cpu.reg16(Cpu::REG16_SP), cpu.m_parameters[0]));
        cpu.setFlagC(IS_CARRY2(cpu.reg16(Cpu::REG16_SP), cpu.m_parameters[0]));
    }

    //..................................................................................................
    void CpuInstr::ldMemImmSp(Cpu& cpu, MemControllerBase& mem)
    {
        cpu.m_instrCycles = 20;
        fetchParam16(cpu, mem);
        uint16_t address = cpu.m_parameters[0] + (cpu.m_parameters[1] << 8);
        mem.write(address, (uint8_t)(cpu.reg16(Cpu::REG16_SP) && 0x00FF));
        mem.write(address + 1, (uint8_t)((cpu.reg16(Cpu::REG16_SP) && 0xFF00) >> 8));
    }

    //..................................................................................................
    void CpuInstr::push(Cpu& cpu, MemControllerBase& mem, Cpu::Reg16 reg)
    {
        cpu.m_instrCycles = 16;
        uint16_t spVal = cpu.reg16(Cpu::REG16_SP);
        mem.write(spVal - 1, (uint8_t)((cpu.reg16(reg) & 0xFF00) >> 8));
        mem.write(spVal - 2, (uint8_t)(cpu.reg16(reg) & 0x00FF));
        cpu.setReg16(Cpu::REG16_SP, spVal - 2);
    }

    //..................................................................................................
    void CpuInstr::pop(Cpu& cpu, MemControllerBase& mem, Cpu::Reg16 reg)
    {
        cpu.m_instrCycles = 12;
        uint16_t spVal = cpu.reg16(Cpu::REG16_SP);
        uint8_t low = mem.read(spVal);
        uint8_t high = mem.read(spVal + 1);
        cpu.setReg16(reg, ((uint16_t)high << 8) + (uint16_t)low);
        cpu.setReg16(Cpu::REG16_SP, spVal + 2);
    }
}