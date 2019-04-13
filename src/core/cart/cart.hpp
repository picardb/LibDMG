#ifndef LIBDMG_CART_HPP
#define LIBDMG_CART_HPP

#include <string>
#include <memory>
#include <exception>

namespace LibDMG
{
    class Cart
    {
    public:
        enum Type
        {
            ROM_ONLY,
            ROM_MBC1,
            ROM_MBC1_RAM,
            ROM_MBC1_RAM_BATT,
            ROM_MBC2,
            ROM_MBC2_BATT,
            ROM_RAM,
            ROM_RAM_BATT,
            ROM_MMM01,
            ROM_MMM01_SRAM,
            ROM_MMM01_SRAM_BATT,
            ROM_MBC3_TIMER_BATT,
            ROM_MBC3_TIMER_RAM_BATT,
            ROM_MBC3,
            ROM_MBC3_RAM,
            ROM_MBC3_RAM_BATT,
            ROM_MBC5,
            ROM_MBC5_RAM,
            ROM_MBC5_RAM_BATT,
            ROM_MBC5_RUMBLE,
            ROM_MBC5_RUMBLE_SRAM,
            ROM_MBC5_RUMBLE_SRAM_BATT,
            POCKET_CAMERA,
            BANDAI_TAMA5,
            HUDSON_HUC3,
            HUDSON_HUC1,
            UNKNOWN
        };

        Cart(const std::string& filePath);

        uint32_t    rawMemSize() const { return m_rawMemSize; }
        std::string titleStr() const { return std::string(reinterpret_cast<const char *>(m_title), CART_SIZE_TITLE); }
        bool        isColorGb() const { return m_isColorGb; }
        bool        isSgb() const { return m_isSgb; }
        Type        type() const { return m_type; }
        std::string typeStr() const { return typeToStr(); }
        uint16_t    romSizeKb() const { return m_romSizeKb; }
        uint8_t     romSizeBanks() const { return m_romSizeBanks; }
        uint8_t     ramSizeKb() const { return m_ramSizeKb; }
        uint8_t     ramSizeBanks() const { return m_ramSizeBanks; }
        
    private:
        static const uint32_t CART_MIN_MEM_SIZE = 0x150;

        static const uint32_t CART_OFFSET_START_CODE        = 0x100;
        static const uint32_t CART_OFFSET_NINTENDO_GRAPHICS = 0x104;
        static const uint32_t CART_OFFSET_TITLE             = 0x134;
        static const uint32_t CART_OFFSET_COLOR_GB          = 0x143;
        static const uint32_t CART_OFFSET_NEW_LICENSEE_CODE = 0x144;
        static const uint32_t CART_OFFSET_SGB               = 0x146;
        static const uint32_t CART_OFFSET_TYPE              = 0x147;
        static const uint32_t CART_OFFSET_ROM_SIZE          = 0x148;
        static const uint32_t CART_OFFSET_RAM_SIZE          = 0x149;

        static const size_t CART_SIZE_START_CODE        = 4;
        static const size_t CART_SIZE_NINTENDO_GRAPHICS = 48;
        static const size_t CART_SIZE_TITLE             = 15;
        static const size_t CART_SIZE_NEW_LICENSEE_CODE = 2;

        std::unique_ptr<uint8_t[]> m_rawMem;
        uint32_t                   m_rawMemSize;

        uint8_t  m_startCode[CART_SIZE_START_CODE];
        uint8_t  m_nintendoGraphics[CART_SIZE_NINTENDO_GRAPHICS];
        uint8_t  m_title[CART_SIZE_TITLE];
        bool     m_isColorGb;
        uint8_t  m_newLicenseeCode[CART_SIZE_NEW_LICENSEE_CODE];
        bool     m_isSgb;
        Type     m_type;
        uint16_t m_romSizeKb;
        uint8_t  m_romSizeBanks;
        uint8_t  m_ramSizeKb;
        uint8_t  m_ramSizeBanks;

        void parseColorGb();
        void parseSgb();
        void parseType();
        void parseRomSize();
        void parseRamSize();
        void parseHeader();
        std::string typeToStr() const;
    };

    class CartException : public std::exception
    {
    public:
        CartException(const std::string& msg)
            : std::exception(),
              m_msg("CartModuleException - " + msg)
        { }
        
        const char* what() const throw() { return m_msg.c_str(); }

    private:
        std::string m_msg;
    };
}

#endif // LIBDMG_CART_HPP