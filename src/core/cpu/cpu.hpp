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
    };
}

#endif // LIBDMG_CPU_HPP