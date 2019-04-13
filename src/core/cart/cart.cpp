#include "cart.hpp"

#include "logger.hpp"

#include <iostream>
#include <fstream>
#include <cstring>

#ifdef _DEBUG
#include <cstdio>
#endif

using namespace std;
using namespace LibDMG;

Cart::Cart(const string& filePath)
{
    // Open file
    ifstream cartFile(filePath.c_str(), ios_base::in | ios_base::binary);
    if (!cartFile.good())
    {
        throw CartException("Invalid file");
    }

    // Get cart memory size
    cartFile.seekg(0, cartFile.end);
    m_rawMemSize = static_cast<uint32_t>(cartFile.tellg());
    cartFile.seekg(0, cartFile.beg);
    if (m_rawMemSize < CART_MIN_MEM_SIZE)
    {
        throw CartException("Not a valid DMG cart");
    }

    // Copy cart content
    m_rawMem = make_unique<uint8_t[]>(m_rawMemSize);
    cartFile.read(reinterpret_cast<char *>(m_rawMem.get()), m_rawMemSize);

    // Close cart
    cartFile.close();

    // Parse header
    parseHeader();
}

void Cart::parseColorGb()
{
    if ((m_rawMem.get()[CART_OFFSET_COLOR_GB] == 0x80)
        || (m_rawMem.get()[CART_OFFSET_COLOR_GB] == 0xC0))
    {
        m_isColorGb = true;
    }
    else
    {
        m_isColorGb = false;
    }
}

void Cart::parseSgb()
{
    if (m_rawMem.get()[CART_OFFSET_SGB] == 0x03)
    {
        m_isSgb = true;
    }
    else
    {
        m_isSgb = false;
    }
}

void Cart::parseType()
{
    switch (m_rawMem.get()[CART_OFFSET_TYPE])
    {
    case 0x00:
        m_type = ROM_ONLY;
        break;
    case 0x01:
        m_type = ROM_MBC1;
        break;
    case 0x02:
        m_type = ROM_MBC1_RAM;
        break;
    case 0x03:
        m_type = ROM_MBC1_RAM_BATT;
        break;
    case 0x05:
        m_type = ROM_MBC2;
        break;
    case 0x06:
        m_type = ROM_MBC2_BATT;
        break;
    case 0x08:
        m_type = ROM_RAM;
        break;
    case 0x09:
        m_type = ROM_RAM_BATT;
        break;
    case 0x0B:
        m_type = ROM_MMM01;
        break;
    case 0x0C:
        m_type = ROM_MMM01_SRAM;
        break;
    case 0x0D:
        m_type = ROM_MMM01_SRAM_BATT;
        break;
    case 0x0F:
        m_type = ROM_MBC3_TIMER_BATT;
        break;
    case 0x10:
        m_type = ROM_MBC3_TIMER_RAM_BATT;
        break;
    case 0x11:
        m_type = ROM_MBC3;
        break;
    case 0x12:
        m_type = ROM_MBC3_RAM;
        break;
    case 0x13:
        m_type = ROM_MBC3_RAM_BATT;
        break;
    case 0x19:
        m_type = ROM_MBC5;
        break;
    case 0x1A:
        m_type = ROM_MBC5_RAM;
        break;
    case 0x1B:
        m_type = ROM_MBC5_RAM_BATT;
        break;
    case 0x1C:
        m_type = ROM_MBC5_RUMBLE;
        break;
    case 0x1D:
        m_type = ROM_MBC5_RUMBLE_SRAM;
        break;
    case 0x1E:
        m_type = ROM_MBC5_RUMBLE_SRAM_BATT;
        break;
    case 0x1F:
        m_type = POCKET_CAMERA;
        break;
    case 0xFD:
        m_type = BANDAI_TAMA5;
        break;
    case 0xFE:
        m_type = HUDSON_HUC3;
        break;
    case 0xFF:
        m_type = HUDSON_HUC1;
        break;
    default:
        m_type = UNKNOWN;
        break;
    }
}

void Cart::parseRomSize()
{
    switch (m_rawMem.get()[CART_OFFSET_ROM_SIZE])
    {
    case 0x00:
        m_romSizeBanks = 2;
        break;

    case 0x01:
        m_romSizeBanks = 4;
        break;

    case 0x02:
        m_romSizeBanks = 8;
        break;

    case 0x03:
        m_romSizeBanks = 16;
        break;

    case 0x04:
        m_romSizeBanks = 32;
        break;

    case 0x05:
        m_romSizeBanks = 64;
        break;

    case 0x06:
        m_romSizeBanks = 128;
        break;

    case 0x52:
        m_romSizeBanks = 72;
        break;

    case 0x53:
        m_romSizeBanks = 80;
        break;

    case 0x54:
        m_romSizeBanks = 96;
        break;

    default:
        m_romSizeBanks = 0;
        break;
    }

    m_romSizeKb = m_romSizeBanks * 16;
}

void Cart::parseRamSize()
{
    switch (m_rawMem.get()[CART_OFFSET_RAM_SIZE])
    {
    case 0x00:
        m_ramSizeBanks = 0;
        m_ramSizeKb = 0;
        break;

    case 0x01:
        m_ramSizeBanks = 1;
        m_ramSizeKb = 2;
        break;

    case 0x02:
        m_ramSizeBanks = 1;
        m_ramSizeKb = 8;
        break;

    case 0x03:
        m_ramSizeBanks = 4;
        m_ramSizeKb = 32;
        break;

    case 0x04:
        m_ramSizeBanks = 16;
        m_ramSizeKb = 128;
        break;

    default:
        m_ramSizeBanks = 0;
        m_ramSizeKb = 0;
        break;
    }
}

void Cart::parseHeader()
{
    memcpy(m_startCode, m_rawMem.get() + CART_OFFSET_START_CODE, CART_SIZE_START_CODE);
    memcpy(m_nintendoGraphics, m_rawMem.get() + CART_OFFSET_NINTENDO_GRAPHICS, CART_SIZE_NINTENDO_GRAPHICS);
    memcpy(m_title, m_rawMem.get() + CART_OFFSET_TITLE, CART_SIZE_TITLE);
    parseColorGb();
    memcpy(m_newLicenseeCode, m_rawMem.get(), CART_SIZE_NEW_LICENSEE_CODE);
    parseSgb();
    parseType();
    parseRomSize();
    parseRamSize();
}

std::string Cart::typeToStr() const
{
    switch (m_type)
    {
    case ROM_ONLY:
        return "ROM only";
    case ROM_MBC1:
        return "ROM + MBC1";
    case ROM_MBC1_RAM:
        return "ROM + MBC1 + RAM";
    case ROM_MBC1_RAM_BATT:
        return "ROM + MBC1 + RAM + BATTERY";
    case ROM_MBC2:
        return "ROM + MBC2";
    case ROM_MBC2_BATT:
        return "ROM + MBC2 + BATTERY";
    case ROM_RAM:
        return "ROM + RAM";
    case ROM_RAM_BATT:
        return "ROM + RAM + BATTERY";
    case ROM_MMM01:
        return "ROM + MMM01";
    case ROM_MMM01_SRAM:
        return "ROM + MMM01 + SRAM";
    case ROM_MMM01_SRAM_BATT:
        return "ROM + MMM01 + SRAM + BATERY";
    case ROM_MBC3_TIMER_BATT:
        return "ROM + MBC3 + TIMER + BATTERY";
    case ROM_MBC3_TIMER_RAM_BATT:
        return "ROM + MBC3 + TIMER + RAM + BATTERY";
    case ROM_MBC3:
        return "ROM + MBC3";
    case ROM_MBC3_RAM:
        return "ROM + MBC3 + RAM";
    case ROM_MBC3_RAM_BATT:
        return "ROM + MBC3 + RAM + BATTERY";
    case ROM_MBC5:
        return "ROM + MBC5";
    case ROM_MBC5_RAM:
        return "ROM + MBC5 + RAM";
    case ROM_MBC5_RAM_BATT:
        return "ROM + MBC5 + RAM + BATTERY";
    case ROM_MBC5_RUMBLE:
        return "ROM + MBC5 + RUMBLE";
    case ROM_MBC5_RUMBLE_SRAM:
        return "ROM + MBC5 + RUMBLE + SRAM";
    case ROM_MBC5_RUMBLE_SRAM_BATT:
        return "ROM + MBC5 + RUMBLE + SRAM + BATTERY";
    case POCKET_CAMERA:
        return "Pocket Camera";
    case BANDAI_TAMA5:
        return "Bandai TAMA5";
    case HUDSON_HUC3:
        return "Hudson Huc-3";
    case HUDSON_HUC1:
        return "Hudson Huc-1";
    case UNKNOWN:
    default:
        return "Unknown";
    }
}