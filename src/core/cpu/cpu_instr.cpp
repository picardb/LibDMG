#include "cpu_instr.hpp"

#include "cpu.hpp"
#include "mem/mem_controller_base.hpp"

namespace LibDMG
{
    //..................................................................................................
    void CpuInstr::nop(Cpu& cpu)
    {
        cpu.m_instrCycles = 4;
    }

    
}