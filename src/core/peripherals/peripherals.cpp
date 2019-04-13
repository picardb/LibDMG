#include "peripherals.hpp"

#include "emulator.hpp"
#include "logger.hpp"

using namespace LibDMG;

void Peripherals::step(int cycles)
{
    m_timer->step(cycles);
    m_lcd->step(cycles);
}

void Peripherals::processInterrupts(void)
{
    if (m_timer->intTimaPending())
    {
        m_timer->clearTIMAPending();
        m_emu->cpu()->setReg16(Cpu::REG16_PC, 12);
    }
}

uint8_t Peripherals::reg(uint8_t offset) const
{
    switch (offset)
    {
    // Input
    case PERIPH_REG_P1:
        // TODO
		LOG_WARN("Periph: Input not implemented yet");
        return 0;

    // Serial
    case PERIPH_REG_SB:
    case PERIPH_REG_SC:
        // TODO
		LOG_WARN("Periph: Serial controller not implemented yet");
        return 0;

    // Timer
    case PERIPH_REG_DIV:  return m_timer->regDIV();
    case PERIPH_REG_TIMA: return m_timer->regTIMA();
    case PERIPH_REG_TMA:  return m_timer->regTMA();
    case PERIPH_REG_TAC:  return m_timer->regTAC();

    // Interrupt flag
    case PERIPH_REG_IF:
        return m_regIF;

    // Sound
    case PERIPH_REG_NR10:
    case PERIPH_REG_NR11:
    case PERIPH_REG_NR12:
    case PERIPH_REG_NR13:
    case PERIPH_REG_NR14:
    case PERIPH_REG_NR21:
    case PERIPH_REG_NR22:
    case PERIPH_REG_NR23:
    case PERIPH_REG_NR24:
    case PERIPH_REG_NR30:
    case PERIPH_REG_NR31:
    case PERIPH_REG_NR32:
    case PERIPH_REG_NR33:
    case PERIPH_REG_NR34:
    case PERIPH_REG_NR41:
    case PERIPH_REG_NR42:
    case PERIPH_REG_NR43:
    case PERIPH_REG_NR44:
    case PERIPH_REG_NR50:
    case PERIPH_REG_NR51:
    case PERIPH_REG_NR52:
        // TODO
		LOG_WARN("Periph: Sound controller not implemented yet");
        return 0;

    // LCD
    case PERIPH_REG_LCDC: return m_lcd->regLCDC();
    case PERIPH_REG_STAT: return m_lcd->regSTAT();
    case PERIPH_REG_SCY:  return m_lcd->regSCY();
    case PERIPH_REG_SCX:  return m_lcd->regSCX();
    case PERIPH_REG_LY:   return m_lcd->regLY();
    case PERIPH_REG_LYC:  return m_lcd->regLYC();
    case PERIPH_REG_DMA:  return m_lcd->regDMA();
    case PERIPH_REG_BGP:  return m_lcd->regBGP();
    case PERIPH_REG_OBP0: return m_lcd->regOBP0();
    case PERIPH_REG_OBP1: return m_lcd->regOBP1();
    case PERIPH_REG_WY:   return m_lcd->regWY();
    case PERIPH_REG_WX:   return m_lcd->regWX();

    // Inerrupt enable;
    case PERIPH_REG_IE:
        return m_regIE;

    default:
        LOG_WARN("Peripherals: Read unkown register");
        return 0;
    }
}

void Peripherals::setReg(uint8_t offset, uint8_t val)
{
    switch (offset)
    {
        // Input
    case PERIPH_REG_P1:
        // TODO
		LOG_WARN("Periph: Input not implemented yet");
        break;

        // Serial
    case PERIPH_REG_SB:
    case PERIPH_REG_SC:
		// TODO
		LOG_WARN("Periph: Serial controller not implemented yet");
        break;

        // Timer
    case PERIPH_REG_DIV:  m_timer->setRegDIV(val); break;
    case PERIPH_REG_TIMA: m_timer->setRegTIMA(val); break;
    case PERIPH_REG_TMA:  m_timer->setRegTMA(val); break;
    case PERIPH_REG_TAC:  m_timer->setRegTAC(val); break;

        // Interrupt flag
    case PERIPH_REG_IF:
        m_regIF = val;
        break;

        // Sound
    case PERIPH_REG_NR10:
    case PERIPH_REG_NR11:
    case PERIPH_REG_NR12:
    case PERIPH_REG_NR13:
    case PERIPH_REG_NR14:
    case PERIPH_REG_NR21:
    case PERIPH_REG_NR22:
    case PERIPH_REG_NR23:
    case PERIPH_REG_NR24:
    case PERIPH_REG_NR30:
    case PERIPH_REG_NR31:
    case PERIPH_REG_NR32:
    case PERIPH_REG_NR33:
    case PERIPH_REG_NR34:
    case PERIPH_REG_NR41:
    case PERIPH_REG_NR42:
    case PERIPH_REG_NR43:
    case PERIPH_REG_NR44:
    case PERIPH_REG_NR50:
    case PERIPH_REG_NR51:
    case PERIPH_REG_NR52:
        // TODO
		LOG_WARN("Periph: Sound controller not implemented yet");
        break;

        // LCD
    case PERIPH_REG_LCDC: m_lcd->setRegLCDC(val); break;
    case PERIPH_REG_STAT: m_lcd->setRegSTAT(val); break;
    case PERIPH_REG_SCY:  m_lcd->setRegSCY(val); break;
    case PERIPH_REG_SCX:  m_lcd->setRegSCX(val); break;
    case PERIPH_REG_LY:   m_lcd->setRegLY(val); break;
    case PERIPH_REG_LYC:  m_lcd->setRegLYC(val); break;
    case PERIPH_REG_DMA:  m_lcd->setRegDMA(val); break;
    case PERIPH_REG_BGP:  m_lcd->setRegBGP(val); break;
    case PERIPH_REG_OBP0: m_lcd->setRegOBP0(val); break;
    case PERIPH_REG_OBP1: m_lcd->setRegOBP1(val); break;
    case PERIPH_REG_WY:   m_lcd->setRegWY(val); break;
    case PERIPH_REG_WX:   m_lcd->setRegWX(val); break;

        // Inerrupt enable;
    case PERIPH_REG_IE:
        m_regIE = val;
        break;

    default:
        LOG_WARN("Peripherals: Write unkown register");
    }
}