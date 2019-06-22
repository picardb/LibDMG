#ifndef LIBDMG_MEM_CONTROLLER_ROM_ONLY_HPP
#define LIBDMG_MEM_CONTROLLER_ROM_ONLY_HPP

#include "mem_controller_base.hpp"
#include "boot_rom.hpp"
#include "logger.hpp"

#include <memory>

namespace LibDMG
{
	// This mem controller is for test only. It can only read in boot ROM.
	class MemControllerRomOnly : public MemControllerBase
	{
	public:
		MemControllerRomOnly(Emulator * emu = nullptr) :
			MemControllerBase(emu),
			m_bootRom(std::make_unique<BootRom>("D:\\Dev\\workspace\\LibDMG\\DMG_ROM.bin"))
		{}

		virtual uint8_t read(uint16_t addr) const;
		virtual void write(uint16_t addr, uint8_t val);

	private:
		std::unique_ptr<BootRom> m_bootRom;
		uint8_t m_videoRam[8 * 1024];
		uint8_t m_mainRam[16 * 1024];
		uint8_t m_oam[160];
		uint8_t m_highRam[127];
	};
}

#endif // LIBDMG_MEM_CONTROLLER_ROM_ONLY_HPP
