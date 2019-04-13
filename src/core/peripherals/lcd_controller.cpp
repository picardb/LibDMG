#include <cstdint>

#include "lcd_controller.hpp"

#include "logger.hpp"

namespace LibDMG
{
void LcdController::step(int cycles)
{
    m_cycles += cycles;

    while (m_cycles >= m_duration)
    {
        m_cycles -= m_duration;
        // End of state
        gotoNextState();
    }
}

void LcdController::gotoNextState(void)
{
    switch (m_state)
    {
    case STATE_MODE0:
        m_regLY++;
        if (m_regLY == 144)
        {
            gotoMode0();
        }
        else
        {
            gotoMode2();
        }
        break;

    case STATE_MODE1:
        m_regLY++;
        if (m_regLY == 154)
        {
            m_regLY = 0;
            gotoMode2();
        }
        else {
            // Stay in mode 0
            gotoMode0();
        }
        break;

    case STATE_MODE2:
        gotoMode3();
        break;

    case STATE_MODE3:
        gotoMode0();
        break;

    default:
        LOG_ERROR("LcdController: Internal error");
        break;
    }
}

} // namespace LibDMG