#include "cpu_instr.hpp"

#include "cpu.hpp"

namespace LibDMG
{
    void CpuInstr::nop(Cpu& cpu)
    {
        cpu.m_instrCycles = 4;
    }
}
