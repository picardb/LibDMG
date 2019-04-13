#include "boot_rom.hpp"

#include <fstream>

using namespace std;
using namespace LibDMG;

BootRom::BootRom(const string& filePath)
{
    // Open file
    ifstream bootFile(filePath.c_str(), ios_base::in | ios_base::binary);
    if (!bootFile.good())
    {
        throw BootRomException("Invalid file");
    }

    // Get file size
    bootFile.seekg(0, bootFile.end);
    if ((uint16_t)bootFile.tellg() != (uint16_t)BOOT_ROM_SIZE) {
        throw BootRomException("Invalid file size");
    }
    bootFile.seekg(0, bootFile.beg);

    // Copy ROM content
    bootFile.read(reinterpret_cast<char *>(m_rawMem), BOOT_ROM_SIZE);

    // Close cart
    bootFile.close();
}