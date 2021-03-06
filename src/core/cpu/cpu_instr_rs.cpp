#include "cpu_instr.hpp"

namespace LibDMG
{
    /**************************************************************************************************/
    /* Rotates & Shifts																				  */
    /**************************************************************************************************/

    //..................................................................................................
    void CpuInstr::rlA(Cpu& cpu, bool rlc)
    {
        cpu.m_instrCycles = 4;
        helperRl(cpu, Cpu::REG8_A, rlc);
    }

    //..................................................................................................
    void CpuInstr::rrA(Cpu& cpu, bool rrc)
    {
        cpu.m_instrCycles = 4;
        helperRr(cpu, Cpu::REG8_A, rrc);
    }
}