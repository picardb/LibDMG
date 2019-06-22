#include "cpu.hpp"

#include <algorithm>

#include "emulator.hpp"
#include "cpu_instr.hpp"
#include "logger.hpp"

using namespace LibDMG;
using namespace std;



//..................................................................................................
void Cpu::step(const Emulator& emu, int cycles)
{
	while (cycles > 0)
	{
		if (m_instrCycles == 0)
		{
			// Process interrupts
			emu.periph()->processInterrupts();

			// Fetch and execute next instruction
			nextInstruction(emu);
		}
		else
		{
			int tmp = std::min(cycles, m_instrCycles);
			m_instrCycles -= tmp;
			cycles -= tmp;
		}
	}
}

void Cpu::setReg16(Reg16 reg, uint16_t val)
{
	switch (reg)
	{
	case REG16_BC:
		setReg8(REG8_B, ((val & 0xFF00) >> 8));
		setReg8(REG8_C, (val & 0xFF));
		break;

	case REG16_DE:
		setReg8(REG8_D, ((val & 0xFF00) >> 8));
		setReg8(REG8_E, (val & 0xFF));
		break;

	case REG16_HL:
		setReg8(REG8_H, ((val & 0xFF00) >> 8));
		setReg8(REG8_L, (val & 0xFF));
		break;

	case REG16_AF:
		setReg8(REG8_A, ((val & 0xFF00) >> 8));
		setReg8(REG8_F, (val & 0xFF));
		break;

	case REG16_PC:
		m_regPC = val;
		break;

	case REG16_SP:
		m_regSP = val;
		break;

	default:
		LOG_WARN("CPU: Writing in wrong register");
	}
}

void Cpu::setFlagZ(bool val)
{
	if (val)
	{
		m_reg8[REG8_F] |= 0x80;
	}
	else
	{
		m_reg8[REG8_F] &= 0x7F;
	}
}

void Cpu::setFlagN(bool val)
{
	if (val)
	{
		m_reg8[REG8_F] |= 0x40;
	}
	else
	{
		m_reg8[REG8_F] &= 0xBF;
	}
}

void Cpu::setFlagH(bool val)
{
	if (val)
	{
		m_reg8[REG8_F] |= 0x20;
	}
	else
	{
		m_reg8[REG8_F] &= 0xDF;
	}
}

void Cpu::setFlagC(bool val)
{
	if (val)
	{
		m_reg8[REG8_F] |= 0x10;
	}
	else
	{
		m_reg8[REG8_F] &= 0xEF;
	}
}

bool Cpu::flagZ(void) const
{
	if ((m_reg8[REG8_F] & 0x80) == 0x80)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Cpu::flagN(void) const
{
	if ((m_reg8[REG8_F] & 0x40) == 0x40)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Cpu::flagH(void) const
{
	if ((m_reg8[REG8_F] & 0x20) == 0x20)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Cpu::flagC(void) const
{
	if ((m_reg8[REG8_F] & 0x10) == 0x10)
	{
		return true;
	}
	else
	{
		return false;
	}
}

uint16_t Cpu::reg16(Reg16 reg) const
{
	switch (reg)
	{
	case LibDMG::Cpu::REG16_BC:
		return ((reg8(REG8_B) << 8) + reg8(REG8_C));

	case LibDMG::Cpu::REG16_DE:
		return ((reg8(REG8_D) << 8) + reg8(REG8_E));

	case LibDMG::Cpu::REG16_HL:
		return ((reg8(REG8_H) << 8) + reg8(REG8_L));

	case LibDMG::Cpu::REG16_AF:
		return ((reg8(REG8_A) << 8) + reg8(REG8_F));

	case LibDMG::Cpu::REG16_PC:
		return m_regPC;

	case LibDMG::Cpu::REG16_SP:
		return m_regSP;

	default:
		LOG_WARN("CPU: Reading in wrong register");
		return 0;
	}
}

void Cpu::nextInstruction(const Emulator& emu)
{
	m_opcode = emu.mem().read(m_regPC);
	m_regPC++;

	switch (m_opcode)
	{
	case 0x00: CpuInstr::nop(*this); break;
    case 0x01: CpuInstr::ldReg16Imm(*this, REG16_BC, emu.mem()); break;
	case 0x02: CpuInstr::ldMemReg8(*this, emu.mem(), REG16_BC, REG8_A);	break;
    case 0x03: CpuInstr::incReg16(*this, REG16_BC); break;
    case 0x04: CpuInstr::incReg8(*this, REG8_B); break;
	case 0x05: CpuInstr::decReg8(*this, REG8_B); break;
	case 0x06: CpuInstr::ldReg8Imm(*this, REG8_B, emu.mem()); break;
    case 0x07: CpuInstr::rlA(*this, true); break;
    case 0x08: CpuInstr::ldMemImmSp(*this, emu.mem()); break;
    case 0x09: CpuInstr::addHlReg16(*this, REG16_BC); break;
	case 0x0A: CpuInstr::ldReg8Mem(*this, REG8_A, emu.mem(), REG16_BC);	break;
    case 0x0B: CpuInstr::decReg16(*this, REG16_BC);	break;
	case 0x0C: CpuInstr::incReg8(*this, REG8_C); break;
	case 0x0D: CpuInstr::decReg8(*this, REG8_C); break;
	case 0x0E: CpuInstr::ldReg8Imm(*this, REG8_C, emu.mem()); break;
    case 0x0F: CpuInstr::rrA(*this, true);	break;
	case 0x11: CpuInstr::ldReg16Imm(*this, REG16_DE, emu.mem()); break;
	case 0x12: CpuInstr::ldMemReg8(*this, emu.mem(), REG16_DE, REG8_A);	break;
    case 0x13: CpuInstr::incReg16(*this, REG16_DE); break;
	case 0x14: CpuInstr::incReg8(*this, REG8_D); break;
	case 0x15: CpuInstr::decReg8(*this, REG8_D); break;
	case 0x16: CpuInstr::ldReg8Imm(*this, REG8_D, emu.mem()); break;
    case 0x17: CpuInstr::rlA(*this, false);	break;
    case 0x18: CpuInstr::jr(*this, emu.mem()); break;
    case 0x19: CpuInstr::addHlReg16(*this, REG16_DE); break;
	case 0x1A: CpuInstr::ldReg8Mem(*this, REG8_A, emu.mem(), REG16_DE); break;
    case 0x1B: CpuInstr::decReg16(*this, REG16_DE);	break;
	case 0x1C: CpuInstr::incReg8(*this, REG8_E); break;
	case 0x1D: CpuInstr::decReg8(*this, REG8_E); break;
	case 0x1E: CpuInstr::ldReg8Imm(*this, REG8_E, emu.mem()); break;
    case 0x1F: CpuInstr::rrA(*this, false); break;
	case 0x20: CpuInstr::jr(*this, emu.mem()); break;
	case 0x21: CpuInstr::ldReg16Imm(*this, REG16_HL, emu.mem()); break;
	case 0x22: CpuInstr::ldiMemReg8(*this, emu.mem(), REG16_HL, REG8_A); break;	break;
    case 0x23: CpuInstr::incReg16(*this, REG16_HL); break;
	case 0x24: CpuInstr::incReg8(*this, REG8_H); break;
	case 0x25: CpuInstr::decReg8(*this, REG8_H); break;
	case 0x26: CpuInstr::ldReg8Imm(*this, REG8_H, emu.mem()); break;
	case 0x28: CpuInstr::jr(*this, emu.mem()); break;
    case 0x29: CpuInstr::addHlReg16(*this, REG16_HL); break;
	case 0x2A: CpuInstr::ldiReg8Mem(*this, REG8_A, emu.mem(), REG16_HL); break;
    case 0x2B: CpuInstr::decReg16(*this, REG16_HL); break;
	case 0x2C: CpuInstr::incReg8(*this, REG8_L); break;
	case 0x2D: CpuInstr::decReg8(*this, REG8_L); break;
	case 0x2E: CpuInstr::ldReg8Imm(*this, REG8_L, emu.mem()); break;
	case 0x30: CpuInstr::jr(*this, emu.mem()); break;
	case 0x31: CpuInstr::ldReg16Imm(*this, REG16_SP, emu.mem()); break;
	case 0x32: CpuInstr::lddMemReg8(*this, emu.mem(), REG16_HL, REG8_A); break;
    case 0x33: CpuInstr::incReg16(*this, REG16_SP);	break;
	case 0x36: CpuInstr::ldMemImm(*this, emu.mem(), REG16_HL); break;
	case 0x38: CpuInstr::jr(*this, emu.mem()); break;
    case 0x39: CpuInstr::addHlReg16(*this, REG16_SP); break;
	case 0x3A: CpuInstr::lddReg8Mem(*this, REG8_A, emu.mem(), REG16_HL); break;
    case 0x3B: CpuInstr::decReg16(*this, REG16_SP);	break;
	case 0x3C: CpuInstr::incReg8(*this, REG8_A); break;
	case 0x3D: CpuInstr::decReg8(*this, REG8_A); break;
	case 0x3E: CpuInstr::ldReg8Imm(*this, REG8_A, emu.mem()); break;
	case 0x40: CpuInstr::ldReg8Reg8(*this, REG8_B, REG8_B); break;
	case 0x41: CpuInstr::ldReg8Reg8(*this, REG8_B, REG8_C); break;
	case 0x42: CpuInstr::ldReg8Reg8(*this, REG8_B, REG8_D); break;
	case 0x43: CpuInstr::ldReg8Reg8(*this, REG8_B, REG8_E); break;
    case 0x44: CpuInstr::ldReg8Reg8(*this, REG8_B, REG8_H); break;
    case 0x45: CpuInstr::ldReg8Reg8(*this, REG8_B, REG8_L); break;
	case 0x46: CpuInstr::ldReg8Mem(*this, REG8_B, emu.mem(), REG16_HL);	break;
    case 0x47: CpuInstr::ldReg8Reg8(*this, REG8_B, REG8_A); break;
    case 0x48: CpuInstr::ldReg8Reg8(*this, REG8_C, REG8_B); break;
    case 0x49: CpuInstr::ldReg8Reg8(*this, REG8_C, REG8_C); break;
	case 0x4A: CpuInstr::ldReg8Reg8(*this, REG8_C, REG8_D); break;
	case 0x4B: CpuInstr::ldReg8Reg8(*this, REG8_C, REG8_E);	break;
	case 0x4C: CpuInstr::ldReg8Reg8(*this, REG8_C, REG8_H);	break;
	case 0x4D: CpuInstr::ldReg8Reg8(*this, REG8_C, REG8_L);	break;
	case 0x4E: CpuInstr::ldReg8Mem(*this, REG8_C, emu.mem(), REG16_HL);	break;
	case 0x4F: CpuInstr::ldReg8Reg8(*this, REG8_C, REG8_A);	break;
	case 0x50: CpuInstr::ldReg8Reg8(*this, REG8_D, REG8_B);	break;
	case 0x51: CpuInstr::ldReg8Reg8(*this, REG8_D, REG8_C);	break;
	case 0x52: CpuInstr::ldReg8Reg8(*this, REG8_D, REG8_D);	break;
	case 0x53: CpuInstr::ldReg8Reg8(*this, REG8_D, REG8_E);	break;
	case 0x54: CpuInstr::ldReg8Reg8(*this, REG8_D, REG8_H);	break;
	case 0x55: CpuInstr::ldReg8Reg8(*this, REG8_D, REG8_L); break;
	case 0x56: CpuInstr::ldReg8Mem(*this, REG8_D, emu.mem(), REG16_HL); break;
	case 0x57: CpuInstr::ldReg8Reg8(*this, REG8_D, REG8_A); break;
	case 0x58: CpuInstr::ldReg8Reg8(*this, REG8_E, REG8_B); break;
	case 0x59: CpuInstr::ldReg8Reg8(*this, REG8_E, REG8_C); break;
	case 0x5A: CpuInstr::ldReg8Reg8(*this, REG8_E, REG8_D); break;
	case 0x5B: CpuInstr::ldReg8Reg8(*this, REG8_E, REG8_E); break;
	case 0x5C: CpuInstr::ldReg8Reg8(*this, REG8_E, REG8_H); break;
	case 0x5D: CpuInstr::ldReg8Reg8(*this, REG8_E, REG8_L); break;
	case 0x5E: CpuInstr::ldReg8Mem(*this, REG8_E, emu.mem(), REG16_HL);	break;
	case 0x5F: CpuInstr::ldReg8Reg8(*this, REG8_E, REG8_A); break;
	case 0x60: CpuInstr::ldReg8Reg8(*this, REG8_H, REG8_B); break;
	case 0x61: CpuInstr::ldReg8Reg8(*this, REG8_H, REG8_C); break;
	case 0x62: CpuInstr::ldReg8Reg8(*this, REG8_H, REG8_D); break;
	case 0x63: CpuInstr::ldReg8Reg8(*this, REG8_H, REG8_E);	break;
	case 0x64: CpuInstr::ldReg8Reg8(*this, REG8_H, REG8_H);	break;
	case 0x65: CpuInstr::ldReg8Reg8(*this, REG8_H, REG8_L);	break;
	case 0x66: CpuInstr::ldReg8Mem(*this, REG8_H, emu.mem(), REG16_HL); break;
	case 0x67: CpuInstr::ldReg8Reg8(*this, REG8_H, REG8_A); break;
	case 0x68: CpuInstr::ldReg8Reg8(*this, REG8_L, REG8_B); break;
	case 0x69: CpuInstr::ldReg8Reg8(*this, REG8_L, REG8_C);	break;
	case 0x6A: CpuInstr::ldReg8Reg8(*this, REG8_L, REG8_D);	break;
	case 0x6B: CpuInstr::ldReg8Reg8(*this, REG8_L, REG8_E);	break;
	case 0x6C: CpuInstr::ldReg8Reg8(*this, REG8_L, REG8_H); break;
	case 0x6D: CpuInstr::ldReg8Reg8(*this, REG8_L, REG8_L); break;
	case 0x6E: CpuInstr::ldReg8Mem(*this, REG8_L, emu.mem(), REG16_HL);	break;
	case 0x6F: CpuInstr::ldReg8Reg8(*this, REG8_L, REG8_A);	break;
	case 0x70: CpuInstr::ldMemReg8(*this, emu.mem(), REG16_HL, REG8_B);	break;
	case 0x71: CpuInstr::ldMemReg8(*this, emu.mem(), REG16_HL, REG8_C); break;
	case 0x72: CpuInstr::ldMemReg8(*this, emu.mem(), REG16_HL, REG8_D);	break;
	case 0x73: CpuInstr::ldMemReg8(*this, emu.mem(), REG16_HL, REG8_E); break;
	case 0x74: CpuInstr::ldMemReg8(*this, emu.mem(), REG16_HL, REG8_H); break;
	case 0x75: CpuInstr::ldMemReg8(*this, emu.mem(), REG16_HL, REG8_L); break;
	case 0x77: CpuInstr::ldMemReg8(*this, emu.mem(), REG16_HL, REG8_A); break;
	case 0x78: CpuInstr::ldReg8Reg8(*this, REG8_A, REG8_B);	break;
	case 0x79: CpuInstr::ldReg8Reg8(*this, REG8_A, REG8_C);	break;
	case 0x7A: CpuInstr::ldReg8Reg8(*this, REG8_A, REG8_D); break;
	case 0x7B: CpuInstr::ldReg8Reg8(*this, REG8_A, REG8_E);	break;
	case 0x7C: CpuInstr::ldReg8Reg8(*this, REG8_A, REG8_H);	break;
	case 0x7D: CpuInstr::ldReg8Reg8(*this, REG8_A, REG8_L);	break;
	case 0x7E: CpuInstr::ldReg8Mem(*this, REG8_A, emu.mem(), REG16_HL);	break;
	case 0x7F: CpuInstr::ldReg8Reg8(*this, REG8_A, REG8_A);	break;
    case 0x80: CpuInstr::addReg8(*this, REG8_B); break;
	case 0x81: CpuInstr::addReg8(*this, REG8_C); break;
	case 0x82: CpuInstr::addReg8(*this, REG8_D); break;
	case 0x83: CpuInstr::addReg8(*this, REG8_E); break;
	case 0x84: CpuInstr::addReg8(*this, REG8_H); break;
	case 0x85: CpuInstr::addReg8(*this, REG8_L); break;
    case 0x86: CpuInstr::addMem(*this, emu.mem(), Cpu::REG16_HL); break;
	case 0x87: CpuInstr::addReg8(*this, REG8_A); break;
	case 0x88: CpuInstr::addReg8(*this, REG8_B, true); break;
	case 0x89: CpuInstr::addReg8(*this, REG8_C, true); break;
	case 0x8A: CpuInstr::addReg8(*this, REG8_D, true); break;
	case 0x8B: CpuInstr::addReg8(*this, REG8_E, true); break;
	case 0x8C: CpuInstr::addReg8(*this, REG8_H, true); break;
	case 0x8D: CpuInstr::addReg8(*this, REG8_L, true); break;
    case 0x8E: CpuInstr::addMem(*this, emu.mem(), REG16_HL, true); break;
	case 0x8F: CpuInstr::addReg8(*this, REG8_A, true); break;
    case 0x90: CpuInstr::subReg8(*this, REG8_B); break;
	case 0x91: CpuInstr::subReg8(*this, REG8_C); break;
	case 0x92: CpuInstr::subReg8(*this, REG8_D); break;
	case 0x93: CpuInstr::subReg8(*this, REG8_E); break;
	case 0x94: CpuInstr::subReg8(*this, REG8_H); break;
	case 0x95: CpuInstr::subReg8(*this, REG8_L); break;
    case 0x96: CpuInstr::subMem(*this, emu.mem(), REG16_HL); break;
	case 0x97: CpuInstr::subReg8(*this, REG8_A); break;
	case 0x98: CpuInstr::subReg8(*this, REG8_B, true); break;
	case 0x99: CpuInstr::subReg8(*this, REG8_C, true); break;
	case 0x9A: CpuInstr::subReg8(*this, REG8_D, true); break;
	case 0x9B: CpuInstr::subReg8(*this, REG8_E, true); break;
	case 0x9C: CpuInstr::subReg8(*this, REG8_H, true); break;
	case 0x9D: CpuInstr::subReg8(*this, REG8_L, true); break;
    case 0x9E: CpuInstr::subMem(*this, emu.mem(), REG16_HL, true); break;
	case 0x9F: CpuInstr::subReg8(*this, REG8_A, true); break;
    case 0xA8: CpuInstr::xorReg8(*this, REG8_B); break;
	case 0xA9: CpuInstr::xorReg8(*this, REG8_C); break;
	case 0xAA: CpuInstr::xorReg8(*this, REG8_D); break;
	case 0xAB: CpuInstr::xorReg8(*this, REG8_E); break;
	case 0xAC: CpuInstr::xorReg8(*this, REG8_H); break;
	case 0xAD: CpuInstr::xorReg8(*this, REG8_L); break;
	case 0xAF: CpuInstr::xorReg8(*this, REG8_A); break;
    case 0xB8: CpuInstr::cpReg8(*this, REG8_B); break;
	case 0xB9: CpuInstr::cpReg8(*this, REG8_C); break;
	case 0xBA: CpuInstr::cpReg8(*this, REG8_D); break;
	case 0xBB: CpuInstr::cpReg8(*this, REG8_E); break;
	case 0xBC: CpuInstr::cpReg8(*this, REG8_H);	break;
	case 0xBD: CpuInstr::cpReg8(*this, REG8_L);	break;
    case 0xBE: CpuInstr::cpMem(*this, emu.mem()); break;
	case 0xBF: CpuInstr::cpReg8(*this, REG8_A);	break;
    case 0xC0: CpuInstr::ret(*this, emu.mem()); break;
    case 0xC1: CpuInstr::pop(*this, emu.mem(), REG16_BC); break;
    case 0xC4: CpuInstr::call(*this, emu.mem()); break;
    case 0xC5: CpuInstr::push(*this, emu.mem(), REG16_BC); break;
    case 0xC6: CpuInstr::addImm(*this, emu.mem()); break;
	case 0xC8: CpuInstr::ret(*this, emu.mem()); break;
	case 0xC9: CpuInstr::ret(*this, emu.mem()); break;
    case 0xCB: CpuInstr::cb(*this, emu.mem()); break;
	case 0xCC: CpuInstr::call(*this, emu.mem()); break;
	case 0xCD: CpuInstr::call(*this, emu.mem()); break;
	case 0xCE: CpuInstr::addImm(*this, emu.mem(), true); break;
	case 0xD0: CpuInstr::ret(*this, emu.mem()); break;
	case 0xD1: CpuInstr::pop(*this, emu.mem(), REG16_DE); break;
	case 0xD4: CpuInstr::call(*this, emu.mem()); break;
	case 0xD5: CpuInstr::push(*this, emu.mem(), REG16_DE); break;
    case 0xD6: CpuInstr::subImm(*this, emu.mem()); break;
	case 0xD8: CpuInstr::ret(*this, emu.mem()); break;
	case 0xDC: CpuInstr::call(*this, emu.mem()); break;
	case 0xDE: CpuInstr::subImm(*this, emu.mem(), true); break;
    case 0xE0: CpuInstr::ldFFnA(*this, emu.mem()); break;
	case 0xE1: CpuInstr::pop(*this, emu.mem(), REG16_HL); break;
    case 0xE2: CpuInstr::ldFFcA(*this, emu.mem()); break;
	case 0xE5: CpuInstr::push(*this, emu.mem(), REG16_HL); break;
    case 0xE8: CpuInstr::addSpImm(*this, emu.mem()); break;
    case 0xEA: CpuInstr::ldMemImmReg8(*this, emu.mem(), REG8_A); break;
    case 0xF0: CpuInstr::ldAFFn(*this, emu.mem()); break;
	case 0xF1: CpuInstr::pop(*this, emu.mem(), REG16_AF); break;
    case 0xF2: CpuInstr::ldAFFc(*this, emu.mem()); break;
	case 0xF5: CpuInstr::push(*this, emu.mem(), REG16_AF); break;
    case 0xF8: CpuInstr::ldHlSpImm(*this, emu.mem()); break;
    case 0xF9: CpuInstr::ldSpHl(*this); break;
	case 0xFA: CpuInstr::ldReg8MemImm(*this, REG8_A, emu.mem()); break;
    case 0xFE: CpuInstr::cpImm(*this, emu.mem()); break;
	default:
		LOG_WARN("CPU: Unknown instruction");
		CpuInstr::nop(*this);
		break;
	}
}