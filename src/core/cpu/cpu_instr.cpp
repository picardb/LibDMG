#include "cpu_instr.hpp"

#include "cpu.hpp"
#include "mem/mem_controller_base.hpp"

namespace LibDMG
{
    //..................................................................................................
    void CpuInstr::fetchParam8(Cpu& cpu, MemControllerBase& mem)
    {
        cpu.m_parameters[0] = mem.read(cpu.m_regPC);
        cpu.m_regPC++;
    }

    //..................................................................................................
    void CpuInstr::fetchParam16(Cpu& cpu, MemControllerBase& mem)
    {
        cpu.m_parameters[0] = mem.read(cpu.m_regPC);
        cpu.m_regPC++;
        cpu.m_parameters[1] = mem.read(cpu.m_regPC);
        cpu.m_regPC++;
    }

    //..................................................................................................
    void CpuInstr::nop(Cpu& cpu)
    {
        cpu.m_instrCycles = 4;
    }

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
}
