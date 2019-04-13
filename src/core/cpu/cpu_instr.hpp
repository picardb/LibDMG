#ifndef LIBDMG_CPU_INSTR_HPP
#define LIBDMG_CPU_INSTR_HPP

namespace LibDMG
{
    class Cpu;

    class CpuInstr
    {
    public:
        static inline void nop(Cpu& cpu);
    };
}

#endif // LIBDMG_CPU_INSTR_HPP