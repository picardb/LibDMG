#ifndef LIBDMG_CPU_HPP
#define LIBDMG_CPU_HPP

#include <cstdint>
#include <exception>
#include <iostream>
#include <array>
#include <cereal/archives/xml.hpp>
#include <cereal/types/array.hpp>

namespace LibDMG
{
    class Emulator;

    class Cpu
    {
    public:
        enum Reg8
        {
            REG8_B = 0,
            REG8_C = 1,
            REG8_D = 2,
            REG8_E = 3,
            REG8_H = 4,
            REG8_L = 5,
            REG8_F = 6,
            REG8_A = 7
        };

        enum Reg16
        {
            REG16_BC,
            REG16_DE,
            REG16_HL,
			REG16_AF,
            REG16_PC,
            REG16_SP
        };

		Cpu() : m_instrCycles(0),
				m_opcode(0),
				m_regSP(0),
				m_regPC(0)
        {
            m_parameters.fill(0);
            m_reg8.fill(0);
        }

        void step(const Emulator& emu, int cycles);

        void saveState(std::ostream& out) { cereal::XMLOutputArchive ar(out); serialize(ar); }
        void loadState(std::istream& in) { cereal::XMLInputArchive ar(in); serialize(ar); }
        template<class Archive>
        void serialize(Archive & ar)
        {
            ar(CEREAL_NVP(m_instrCycles), 
                CEREAL_NVP(m_opcode),
                CEREAL_NVP(m_parameters),
                CEREAL_NVP(m_reg8),
                CEREAL_NVP(m_regPC),
                CEREAL_NVP(m_regSP)
            );
        }

        void setReg8(Reg8 reg, uint8_t val) { m_reg8[reg] = val; }
        void setReg16(Reg16 reg, uint16_t val);
		void setFlagZ(bool val);
		void setFlagN(bool val);
		void setFlagH(bool val);
		void setFlagC(bool val);

        uint8_t reg8(Reg8 reg) const { return m_reg8[reg]; }
        uint16_t reg16(Reg16 reg) const;
		bool flagZ(void) const;
		bool flagN(void) const;
		bool flagH(void) const;
		bool flagC(void) const;

    private:
		friend class CpuInstr;

        int		m_instrCycles;
		uint8_t m_opcode;
		std::array<uint8_t, 2> m_parameters;
        std::array<uint8_t, 8> m_reg8;
        uint16_t m_regPC;
        uint16_t m_regSP;

        void nextInstruction(const Emulator& emu);

		void instrLdReg8Mem(Reg8 reg, Reg16 addr);
		void instrLddReg8Mem(Reg8 reg, Reg16 addr);
		void instrLdiReg8Mem(Reg8 reg, Reg16 addr);
		void instrLdReg8MemImm(Reg8 reg);
		void instrLdMemReg8(Reg16 addr, Reg8 reg);
		void instrLddMemReg8(Reg16 addr, Reg8 reg);
		void instrLdiMemReg8(Reg16 addr, Reg8 reg);
		void instrLdMemImm(Reg16 addr);
		void instrLdMemImmReg8(Reg8 reg);
		void instrLdAFFc(void);
		void instrLdFFcA(void);
		void instrLdAFFn(void);
		void instrLdFFnA(void);

		// 16-bit loads
		void instrLdReg16Imm(Reg16 reg);
		void instrLdSpHl(void);
		void instrLdHlSpImm(void);
		void instrLdMemImmSp(void);
		void instrPush(Reg16 reg);
		void instrPop(Reg16 reg);

		// 8-bit ALU
		void instrAddReg8(Reg8 reg, bool carry = false);
		void instrAddMem(Reg16 reg, bool carry = false);
		void instrAddImm(bool carry = false);
		void instrSubReg8(Reg8 reg, bool carry = false);
		void instrSubMem(Reg16 reg, bool carry = false);
		void instrSubImm(bool carry = false);
		void instrXorReg8(Reg8 reg);
		void instrIncReg8(Reg8 reg);
		void instrDecReg8(Reg8 reg);
		void instrCpReg8(Reg8 reg);
		void instrCpMem(void);
		void instrCpImm(void);

		// 16-bit ALU
		void instrIncReg16(Reg16 reg);
		void instrDecReg16(Reg16 reg);
		void instrAddHlReg16(Reg16 reg);
		void instrAddSpImm(void);

		// Jumps
		void instrJr(void);
		void instrCall(void);
		void instrRet(void);

		// Rotates and shifts
		void instrRlA(bool rlc);
		void instrRrA(bool rrc);

		void instrCbMain(void);
		void instrCbBitReg8(Reg8 reg, uint8_t index);
		void instrCbBitMem8(uint8_t index);
		void instrCbResReg8(Reg8 reg, uint8_t index);
		void instrCbResMem8(uint8_t index);
		void instrCbSetReg8(Reg8 reg, uint8_t index);
		void instrCbSetMem8(uint8_t index);
		void instrCbRlReg8(Reg8 reg, bool rlc = false);
		void instrCbRlMem(bool rlc = false);
		void instrCbRrReg8(Reg8 reg, bool rrc = false);
		void instrCbRrMem(bool rrc = false);

		void instrHelperRl(Reg8 reg, bool rlc);
		void instrHelperRr(Reg8 reg, bool rrc);
    };
}

#endif // LIBDMG_CPU_HPP