#include "cpu_instr.hpp"

#include "cpu/cpu.hpp"
#include "mem/mem_controller_base.hpp"

namespace LibDMG
{
    /**************************************************************************************************/
    /* 8-bit loads                                                                                    */
    /**************************************************************************************************/

    //..................................................................................................
    void CpuInstr::ldReg8Imm(Cpu& cpu, Cpu::Reg8 reg, MemControllerBase& mem)
    {
        cpu.m_instrCycles = 8;
        fetchParam8(cpu, mem);
        cpu.setReg8(reg, cpu.m_parameters[0]);
    }

    //..................................................................................................
    void CpuInstr::ldReg8Reg8(Cpu& cpu, Cpu::Reg8 dest, Cpu::Reg8 src)
    {
        cpu.m_instrCycles = 4;
        cpu.setReg8(dest, cpu.reg8(src));
    }

    //..................................................................................................
    void CpuInstr::ldReg8Mem(Cpu& cpu, Cpu::Reg8 reg, MemControllerBase& mem, Cpu::Reg16 addr)
    {
        cpu.m_instrCycles = 8;
        cpu.setReg8(reg, mem.read(cpu.reg16(addr)));
    }

    //..................................................................................................
    void CpuInstr::lddReg8Mem(Cpu& cpu, Cpu::Reg8 reg, MemControllerBase& mem, Cpu::Reg16 addr)
    {
        ldReg8Mem(cpu, reg, mem, addr);
        cpu.setReg16(addr, cpu.reg16(addr) - 1);
    }

    //..................................................................................................
    void CpuInstr::ldiReg8Mem(Cpu& cpu, Cpu::Reg8 reg, MemControllerBase& mem, Cpu::Reg16 addr)
    {
        ldReg8Mem(cpu, reg, mem, addr);
        cpu.setReg16(addr, cpu.reg16(addr) + 1);
    }

    //..................................................................................................
    void CpuInstr::ldReg8MemImm(Cpu& cpu, Cpu::Reg8 reg, MemControllerBase& mem)
    {
        cpu.m_instrCycles = 16;
        fetchParam16(cpu, mem);
        uint16_t addr = cpu.m_parameters[0] + (cpu.m_parameters[1] << 8);
        cpu.setReg8(reg, mem.read(addr));
    }

    //..................................................................................................
    void CpuInstr::ldMemReg8(Cpu& cpu, MemControllerBase& mem, Cpu::Reg16 addr, Cpu::Reg8 reg)
    {
        cpu.m_instrCycles = 8;
        uint16_t address = cpu.reg16(addr);
        mem.write(address, cpu.reg8(reg));
    }

    //..................................................................................................
    void CpuInstr::lddMemReg8(Cpu& cpu, MemControllerBase& mem, Cpu::Reg16 addr, Cpu::Reg8 reg)
    {
        ldMemReg8(cpu, mem, addr, reg);
        cpu.setReg16(addr, cpu.reg16(addr) - 1);
    }

    //..................................................................................................
    void CpuInstr::ldiMemReg8(Cpu& cpu, MemControllerBase& mem, Cpu::Reg16 addr, Cpu::Reg8 reg)
    {
        ldMemReg8(cpu, mem, addr, reg);
        cpu.setReg16(addr, cpu.reg16(addr) + 1);
    }

    //..................................................................................................
    void CpuInstr::ldMemImm(Cpu& cpu, MemControllerBase& mem, Cpu::Reg16 addr)
    {
        cpu.m_instrCycles = 12;
        fetchParam8(cpu, mem);
        mem.write(cpu.reg16(addr), cpu.m_parameters[0]);
    }

    //..................................................................................................
    void CpuInstr::ldMemImmReg8(Cpu& cpu, MemControllerBase& mem, Cpu::Reg8 reg)
    {
        cpu.m_instrCycles = 16;
        fetchParam16(cpu, mem);
        mem.write(cpu.m_parameters[0] + (cpu.m_parameters[1] << 8), cpu.reg8(reg));
    }

    //..................................................................................................
    void CpuInstr::ldAFFc(Cpu& cpu, MemControllerBase& mem)
    {
        cpu.m_instrCycles = 8;
        cpu.setReg8(Cpu::REG8_A, mem.read(0xFF00 + cpu.reg8(Cpu::REG8_C)));
    }

    //..................................................................................................
    void CpuInstr::ldFFcA(Cpu& cpu, MemControllerBase& mem)
    {
        cpu.m_instrCycles = 8;
        mem.write(0xFF00 + cpu.reg8(Cpu::REG8_C), cpu.reg8(Cpu::REG8_A));
    }

    //..................................................................................................
    void CpuInstr::ldAFFn(Cpu& cpu, MemControllerBase& mem)
    {
        cpu.m_instrCycles = 12;
        fetchParam8(cpu, mem);
        cpu.setReg8(Cpu::REG8_A, mem.read(0xFF00 + cpu.m_parameters[0]));
    }

    //..................................................................................................
    void CpuInstr::ldFFnA(Cpu& cpu, MemControllerBase& mem)
    {
        cpu.m_instrCycles = 12;
        fetchParam8(cpu, mem);
        mem.write(0xFF00 + cpu.m_parameters[0], cpu.reg8(Cpu::REG8_A));
    }
}