#ifndef LIBDMG_MEM_CONTROLLER_BASE_HPP
#define LIBDMG_MEM_CONTROLLER_BASE_HPP

#include <cstdint>

namespace LibDMG 
{
    class Emulator;

    class MemControllerBase
    {
    public:
        MemControllerBase(Emulator * emu = nullptr) :
            m_emu(emu)
        {}

        virtual uint8_t read(uint16_t addr) const = 0;
        virtual void write(uint16_t addr, uint8_t val) = 0;

    protected:
        Emulator * m_emu;
    };

}

#endif // LIBDMG_MEM_CONTROLLER_BASE_HPP
