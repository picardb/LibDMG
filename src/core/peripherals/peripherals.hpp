#ifndef LIBDMG_PERIPHERALS_HPP
#define LIBDMG_PERIPHERALS_HPP

#include <cstdint>
#include <memory>
#include <cereal/archives/xml.hpp>
#include <cereal/types/memory.hpp>

#include "timer.hpp"
#include "lcd_controller.hpp"

namespace LibDMG
{
    class Emulator;

    class Peripherals
    {
    public:
        Peripherals(Emulator * emu = nullptr) : 
            m_emu(emu),
            m_timer(std::make_unique<Timer>()),
            m_lcd(std::make_unique<LcdController>())
             
        {}

        void step(int cycles);

        void saveState(std::ostream& out) { cereal::XMLOutputArchive ar(out); serialize(ar); }
        void loadState(std::istream& in) { cereal::XMLInputArchive ar(in); serialize(ar); }
        template<class Archive>
        void serialize(Archive & ar)
        {
            ar(CEREAL_NVP(m_timer),
               CEREAL_NVP(m_lcd),
               CEREAL_NVP(m_regIF),
               CEREAL_NVP(m_regIE),
               CEREAL_NVP(m_flagIME));
        }

        void processInterrupts(void);

        uint8_t regIF() const { return m_regIF; }
        uint8_t regIE() const { return m_regIE; }
        bool flagIME() const { return m_flagIME; }
        uint8_t reg(uint8_t offset) const;

        void setRegIF(uint8_t val) { m_regIF = val; }
        void setRegIE(uint8_t val) { m_regIE = val; }
        void setFlagIME(bool val) { m_flagIME = val; }
        void setReg(uint8_t offset, uint8_t val);

        enum RegOffset
        {
            PERIPH_REG_P1 = 0x00,
            PERIPH_REG_SB = 0x01,
            PERIPH_REG_SC = 0x02,
            PERIPH_REG_DIV = 0x04,
            PERIPH_REG_TIMA = 0x05,
            PERIPH_REG_TMA = 0x06,
            PERIPH_REG_TAC = 0x07,
            PERIPH_REG_IF = 0x0F,
            PERIPH_REG_NR10 = 0x10,
            PERIPH_REG_NR11 = 0x11,
            PERIPH_REG_NR12 = 0x12,
            PERIPH_REG_NR13 = 0x13,
            PERIPH_REG_NR14 = 0x14,
            PERIPH_REG_NR21 = 0x16,
            PERIPH_REG_NR22 = 0x17,
            PERIPH_REG_NR23 = 0x18,
            PERIPH_REG_NR24 = 0x19,
            PERIPH_REG_NR30 = 0x1A,
            PERIPH_REG_NR31 = 0x1B,
            PERIPH_REG_NR32 = 0x1C,
            PERIPH_REG_NR33 = 0x1D,
            PERIPH_REG_NR34 = 0x1E,
            PERIPH_REG_NR41 = 0x20,
            PERIPH_REG_NR42 = 0x21,
            PERIPH_REG_NR43 = 0x22,
            PERIPH_REG_NR44 = 0x23,
            PERIPH_REG_NR50 = 0x24,
            PERIPH_REG_NR51 = 0x25,
            PERIPH_REG_NR52 = 0x26,
            PERIPH_REG_LCDC = 0x40,
            PERIPH_REG_STAT = 0x41,
            PERIPH_REG_SCY = 0x42,
            PERIPH_REG_SCX = 0x43,
            PERIPH_REG_LY = 0x44,
            PERIPH_REG_LYC = 0x45,
            PERIPH_REG_DMA = 0x46,
            PERIPH_REG_BGP = 0x47,
            PERIPH_REG_OBP0 = 0x48,
            PERIPH_REG_OBP1 = 0x49,
            PERIPH_REG_WY = 0x4A,
            PERIPH_REG_WX = 0x4B,
            PERIPH_REG_IE = 0xFF
        };

    private:
        Emulator * m_emu;

        std::unique_ptr<Timer>		   m_timer;
		std::unique_ptr<LcdController> m_lcd;

        uint8_t m_regIF;    // $FF0F - Interrupt Flag
        uint8_t m_regIE;    // $FFFF - Interrupt Enable
        bool    m_flagIME;  // Interrupt Master Enable
    };
}

#endif // LIBDMG_PERIPHERALS_HPP
