#ifndef LIBDMG_BOOT_ROM_HPP
#define LIBDMG_BOOT_ROM_HPP

#include <cstdint>
#include <string>
#include <exception>

namespace LibDMG
{
    class BootRom
    {
    public:
        BootRom(const std::string& filePath);

        uint8_t read(uint16_t addr) const { return m_rawMem[addr]; }

    private:
        static const size_t BOOT_ROM_SIZE = 256;

        uint8_t m_rawMem[BOOT_ROM_SIZE];
    };

    class BootRomException : public std::exception
    {
    public:
        BootRomException(const std::string& msg)
            : std::exception(),
            m_msg("BootRomModuleException - " + msg)
        { }

        const char* what() const throw() { return m_msg.c_str(); }

    private:
        std::string m_msg;
    };
}

#endif // LIBDMG_BOOT_ROM_HPP