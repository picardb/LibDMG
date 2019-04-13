#ifndef LIBDMG_CPU_INSTR_HPP
#define LIBDMG_CPU_INSTR_HPP

namespace LibDMG
{
    class Cpu;
    class MemControllerBase;

    class CpuInstr
    {
    public:
        static void nop(Cpu& cpu);

        // 8-bit loads
        static void ldReg8Imm(Cpu& cpu, Cpu::Reg8 reg, MemControllerBase& mem);
        static void ldReg8Reg8(Cpu& cpu, Cpu::Reg8 dest, Cpu::Reg8 src);

    private:
        static void fetchParam8(Cpu& cpu, MemControllerBase& mem);
        static void fetchParam16(Cpu& cpu, MemControllerBase& mem);
    };
}

#endif // LIBDMG_CPU_INSTR_HPP