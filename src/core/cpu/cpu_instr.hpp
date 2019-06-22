#ifndef LIBDMG_CPU_INSTR_HPP
#define LIBDMG_CPU_INSTR_HPP

#include "cpu/cpu.hpp"
#include "mem/mem_controller_base.hpp"

namespace LibDMG
{
    class CpuInstr
    {
    public:
        static void nop(Cpu& cpu);
        static void cb(Cpu& cpu, MemControllerBase& mem);

        // Helper functions
        static void helperRl(Cpu& cpu, Cpu::Reg8 reg, bool rlc);
        static void helperRr(Cpu& cpu, Cpu::Reg8 reg, bool rrc);

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
        static void xorReg8(Cpu& cpu, Cpu::Reg8 reg);
        static void incReg8(Cpu& cpu, Cpu::Reg8 reg);
        static void decReg8(Cpu& cpu, Cpu::Reg8 reg);
        static void cpReg8(Cpu& cpu, Cpu::Reg8 reg);
        static void cpMem(Cpu& cpu, MemControllerBase& mem);
        static void cpImm(Cpu& cpu, MemControllerBase& mem);

        // 16-bit ALU: cpu_instr_alu16.cpp
        static void incReg16(Cpu& cpu, Cpu::Reg16 reg);
        static void decReg16(Cpu& cpu, Cpu::Reg16 reg);
        static void addHlReg16(Cpu& cpu, Cpu::Reg16 reg);
        static void addSpImm(Cpu& cpu, MemControllerBase& mem);

        // Jumps: cpu_instr_jmp.cpp
        static void jr(Cpu& cpu, MemControllerBase& mem);
        static void call(Cpu& cpu, MemControllerBase& mem);
        static void ret(Cpu& cpu, MemControllerBase& mem);

        // Rotates and shifts: cpu_instr_rs.cpp
        static void rlA(Cpu& cpu, bool rlc);
        static void rrA(Cpu& cpu, bool rrc);

        // CB-extended instructions: cpu_instr_cb.cpp
        static void cbBitReg8(Cpu& cpu, Cpu::Reg8 reg, uint8_t index);
        static void cbBitMem8(Cpu& cpu, MemControllerBase& mem, uint8_t index);
        static void cbResReg8(Cpu& cpu, Cpu::Reg8 reg, uint8_t index);
        static void cbResMem8(Cpu& cpu, MemControllerBase& mem, uint8_t index);
        static void cbSetReg8(Cpu& cpu, Cpu::Reg8 reg, uint8_t index);
        static void cbSetMem8(Cpu& cpu, MemControllerBase& mem, uint8_t index);
        static void cbRlReg8(Cpu& cpu, Cpu::Reg8 reg, bool rlc = false);
        static void cbRlMem(Cpu& cpu, MemControllerBase& mem, bool rlc = false);
        static void cbRrReg8(Cpu& cpu, Cpu::Reg8 reg, bool rrc = false);
        static void cbRrMem(Cpu& cpu, MemControllerBase& mem, bool rrc = false);

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