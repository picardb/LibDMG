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

        // 8-bit loads: cpu_instr_ld8.cpp
        static void ldReg8Imm(Cpu& cpu, Cpu::Reg8 reg, MemControllerBase& mem);
        static void ldReg8Reg8(Cpu& cpu, Cpu::Reg8 dest, Cpu::Reg8 src);
        static void ldReg8Mem(Cpu& cpu, Cpu::Reg8 reg, MemControllerBase& mem, Cpu::Reg16 addr);
        static void lddReg8Mem(Cpu& cpu, Cpu::Reg8 reg, MemControllerBase& mem, Cpu::Reg16 addr);
        static void ldiReg8Mem(Cpu& cpu, Cpu::Reg8 reg, MemControllerBase& mem, Cpu::Reg16 addr);
        static void ldReg8MemImm(Cpu& cpu, Cpu::Reg8 reg, MemControllerBase& mem);
        static void ldMemReg8(Cpu& cpu, MemControllerBase& mem, Cpu::Reg16 addr, Cpu::Reg8 reg);
        static void lddMemReg8(Cpu& cpu, MemControllerBase& mem, Cpu::Reg16 addr, Cpu::Reg8 reg);
        static void ldiMemReg8(Cpu& cpu, MemControllerBase& mem, Cpu::Reg16 addr, Cpu::Reg8 reg);
		static void ldMemImm(Cpu& cpu, MemControllerBase& mem, Cpu::Reg16 addr);
        static void ldMemImmReg8(Cpu& cpu, MemControllerBase& mem, Cpu::Reg8 reg);
        static void ldAFFc(Cpu& cpu, MemControllerBase& mem);
        static void ldFFcA(Cpu& cpu, MemControllerBase& mem);
        static void ldAFFn(Cpu& cpu, MemControllerBase& mem);
        static void ldFFnA(Cpu& cpu, MemControllerBase& mem);

        // 16-bit loads: cpu_instr_ld16.cpp
        static void ldReg16Imm(Cpu& cpu, Cpu::Reg16 reg, MemControllerBase& mem);
        static void ldSpHl(Cpu& cpu);
        static void ldHlSpImm(Cpu& cpu, MemControllerBase& mem);
        static void ldMemImmSp(Cpu& cpu, MemControllerBase& mem);
        static void push(Cpu& cpu, MemControllerBase& mem, Cpu::Reg16 reg);
        static void pop(Cpu& cpu, MemControllerBase& mem, Cpu::Reg16 reg);

        // 8-bit ALU: cpu_instr_alu8.cpp
        static void addReg8(Cpu& cpu, Cpu::Reg8 reg, bool carry = false);
        static void addMem(Cpu& cpu, MemControllerBase& mem, Cpu::Reg16 reg, bool carry = false);
        static void addImm(Cpu& cpu, MemControllerBase& mem, bool carry = false);
        static void subReg8(Cpu& cpu, Cpu::Reg8 reg, bool carry = false);
        static void subMem(Cpu& cpu, MemControllerBase& mem, Cpu::Reg16 reg, bool carry = false);
        static void subImm(Cpu& cpu, MemControllerBase& mem, bool carry = false);

    private:

        //..................................................................................................
        static void fetchParam8(Cpu& cpu, MemControllerBase& mem)
        {
            cpu.m_parameters[0] = mem.read(cpu.m_regPC);
            cpu.m_regPC++;
        }

        //..................................................................................................
        static void fetchParam16(Cpu& cpu, MemControllerBase& mem)
        {
            cpu.m_parameters[0] = mem.read(cpu.m_regPC);
            cpu.m_regPC++;
            cpu.m_parameters[1] = mem.read(cpu.m_regPC);
            cpu.m_regPC++;
        }
    };
}

#endif // LIBDMG_CPU_INSTR_HPP