#include "emulator.hpp"

using namespace LibDMG;
using namespace std;

Emulator::Emulator()
{
    m_cpu = make_unique<Cpu>(this);
    m_periph = make_unique<Peripherals>(this);
    m_mem = make_unique<MemControllerRomOnly>(this);
}

void Emulator::step(int cycles)
{
    m_cpu->step(cycles);
    m_periph->step(cycles);
}