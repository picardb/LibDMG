#include "emulator.hpp"

using namespace LibDMG;
using namespace std;

Emulator::Emulator()
{
    m_cpu = make_unique<Cpu>();
    m_periph = make_unique<Peripherals>(this);
    m_mem = make_unique<MemControllerRomOnly>(this);
}

void Emulator::step(int cycles)
{
    m_cpu->step(*this, cycles);
    m_periph->step(cycles);
}