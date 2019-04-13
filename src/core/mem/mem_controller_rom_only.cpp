#include "mem_controller_rom_only.hpp"

#include "emulator.hpp"

namespace LibDMG
{
uint8_t MemControllerRomOnly::read(uint16_t addr) const
{
    // Boot ROM
    if (addr < 0x100)
    {
        return m_bootRom->read(addr);
    }
    // ROM
    else if (addr < 0x8000)
    {
        LOG_WARN("MemControllerRomOnly: cart ROM not implemented");
        return 0;
    }
    // Video RAM
    else if (addr < 0xA000)
    {
        return m_videoRam[addr - 0x8000];
    }
    // Switchable RAM
    else if (addr < 0xC000)
    {
        LOG_WARN("MemControllerRomOnly: switchable RAM not implemented");
    }
    // Main RAM
    else if (addr < 0xE000)
    {
        return m_mainRam[addr - 0xC000];
    }
    // Main RAM echo
    else if (addr < 0xFE00)
    {
        return m_mainRam[addr - 0xE000];
    }
    // OAM
    else if (addr < 0xFEA0)
    {
        return m_oam[addr - 0xFEA0];
    }
    // Reserved area
    else if (addr < 0xFF00)
    {
        LOG_WARN("MemControllerRomOnly: trying to write in reserved area");
    }
    // I/O registers
    else if (addr < 0xFF4C)
    {
        return m_emu->periph()->reg(addr - 0xFF00);
    }
    // Reserved
    else if (addr < 0xFF80)
    {
        LOG_WARN("MemControllerRomOnly: trying to write in reserved area");
    }
    // "High" RAM
    else if (addr < 0xFFFF)
    {
        return m_highRam[addr - 0xFF80];
    }
    // Interrupt Enable register
    else if (addr == 0xFFFF)
    {
        return m_emu->periph()->regIE();
    }
}

void MemControllerRomOnly::write(uint16_t addr, uint8_t val)
{
    // ROM
    if (addr < 0x8000)
    {
        LOG_WARN("MemControllerRomOnly: can't write in ROM yet");
    }
    // Video RAM
    else if (addr < 0xA000)
    {
        m_videoRam[addr - 0x8000] = val;
    }
    // Switchable RAM
    else if (addr < 0xC000)
    {
        LOG_WARN("MemControllerRomOnly: switchable RAM not implemented");
    }
    // Main RAM
    else if (addr < 0xE000)
    {
        m_mainRam[addr - 0xC000] = val;
    }
    // Main RAM echo
    else if (addr < 0xFE00)
    {
        m_mainRam[addr - 0xE000] = val;
    }
    // OAM
    else if (addr < 0xFEA0)
    {
        m_oam[addr - 0xFEA0] = val;
    }
    // Reserved area
    else if (addr < 0xFF00)
    {
        LOG_WARN("MemControllerRomOnly: trying to write in reserved area");
    }
    // I/O registers
    else if (addr < 0xFF4C)
    {
        m_emu->periph()->setReg(addr - 0xFF00, val);
    }
    // Reserved
    else if (addr < 0xFF80)
    {
        LOG_WARN("MemControllerRomOnly: trying to write in reserved area");
    }
    // "High" RAM
    else if (addr < 0xFFFF)
    {
        m_highRam[addr - 0xFF80] = val;
    }
    // Interrupt Enable register
    else if (addr == 0xFFFF)
    {
        m_emu->periph()->setRegIE(val);
    }
}
} // namespace LibDMG