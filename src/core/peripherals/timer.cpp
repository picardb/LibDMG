#include "timer.hpp"
#include "logger.hpp"

using namespace LibDMG;

void Timer::step(int cycles)
{
    m_divPreCounter += cycles;

    // Update DIV counter
    while (m_divPreCounter >= TIMER_DIV_PRESCALER) {
        m_divPreCounter -= TIMER_DIV_PRESCALER;
        m_regDIV++;
    }

    if (!m_isRunning) {
        return;
    }

    m_timaPreCounter += cycles;

    // Update TIMA counter
    while (m_timaPreCounter >= m_timaPrescaler) {
        m_timaPreCounter -= m_timaPrescaler;
        m_regTIMA++;
        // Overflow
        if (m_regTIMA == 0) {
            m_regTIMA = m_regTMA;
            m_intTIMAPending = true;
        }
    }
}

void Timer::setRegDIV(uint8_t val)
{
    m_regDIV = 0;

    // Resetting DIV also resets internal counters
    m_divPreCounter = 0;
    m_timaPreCounter = 0;
}

void Timer::setRegTAC(uint8_t val)
{
    m_regTAC = val;

    switch (val & 0x03)
    {
    case 0x00:
        m_timaPrescaler = 1024;
        break;

    case 0x01:
        m_timaPrescaler = 16;
        break;

    case 0x02:
        m_timaPrescaler = 64;
        break;

    case 0x03:
        m_timaPrescaler = 256;
        break;

    default:
        // TODO throw exception
        break;
    }

    if ((m_regTAC & 0x04) == 0x04) {
        m_isRunning = true;
    }
    else {
        m_isRunning = false;
    }
}