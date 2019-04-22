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
	case 0x03:
		instrIncReg16(REG16_BC);
		break;
    case 0x04: CpuInstr::incReg8(*this, REG8_B); break;
	case 0x05: CpuInstr::decReg8(*this, REG8_B); break;
	case 0x06: CpuInstr::ldReg8Imm(*this, REG8_B, emu.mem()); break;
	case 0x07:
		instrRlA(true);
		break;
    case 0x08: CpuInstr::ldMemImmSp(*this, emu.mem()); break;
	case 0x09:
		instrAddHlReg16(REG16_BC);
		break;
	case 0x0A: CpuInstr::ldReg8Mem(*this, REG8_A, emu.mem(), REG16_BC);	break;
	case 0x0B:
		instrDecReg16(REG16_BC);
		break;
	case 0x0C: CpuInstr::incReg8(*this, REG8_C); break;
	case 0x0D: CpuInstr::decReg8(*this, REG8_C); break;
	case 0x0E: CpuInstr::ldReg8Imm(*this, REG8_C, emu.mem()); break;
	case 0x0F:
		instrRrA(true);
		break;
	case 0x11: CpuInstr::ldReg16Imm(*this, REG16_DE, emu.mem()); break;
	case 0x12: CpuInstr::ldMemReg8(*this, emu.mem(), REG16_DE, REG8_A);	break;
	case 0x13:
		instrIncReg16(REG16_DE);
		break;
	case 0x14: CpuInstr::incReg8(*this, REG8_D); break;
	case 0x15: CpuInstr::decReg8(*this, REG8_D); break;
	case 0x16: CpuInstr::ldReg8Imm(*this, REG8_D, emu.mem()); break;
	case 0x17:
		instrRlA(false);
		break;
	case 0x18:
		instrJr();
		break;
	case 0x19:
		instrAddHlReg16(REG16_DE);
		break;
	case 0x1A: CpuInstr::ldReg8Mem(*this, REG8_A, emu.mem(), REG16_DE); break;
	case 0x1B:
		instrDecReg16(REG16_DE);
		break;
	case 0x1C: CpuInstr::incReg8(*this, REG8_E); break;
	case 0x1D: CpuInstr::decReg8(*this, REG8_E); break;
	case 0x1E: CpuInstr::ldReg8Imm(*this, REG8_E, emu.mem()); break;
	case 0x1F:
		instrRrA(false);
		break;
	case 0x20:
		instrJr();
		break;
	case 0x21: CpuInstr::ldReg16Imm(*this, REG16_HL, emu.mem()); break;
	case 0x22: CpuInstr::ldiMemReg8(*this, emu.mem(), REG16_HL, REG8_A); break;	break;
	case 0x23:
		instrIncReg16(REG16_HL);
		break;
	case 0x24: CpuInstr::incReg8(*this, REG8_H); break;
	case 0x25: CpuInstr::decReg8(*this, REG8_H); break;
	case 0x26: CpuInstr::ldReg8Imm(*this, REG8_H, emu.mem()); break;
	case 0x28:
		instrJr();
		break;
	case 0x29:
		instrAddHlReg16(REG16_HL);
		break;
	case 0x2A: CpuInstr::ldiReg8Mem(*this, REG8_A, emu.mem(), REG16_HL); break;
	case 0x2B:
		instrDecReg16(REG16_HL);
		break;
	case 0x2C: CpuInstr::incReg8(*this, REG8_L); break;
	case 0x2D: CpuInstr::decReg8(*this, REG8_L); break;
	case 0x2E: CpuInstr::ldReg8Imm(*this, REG8_L, emu.mem()); break;
	case 0x30:
		instrJr();
		break;
	case 0x31: CpuInstr::ldReg16Imm(*this, REG16_SP, emu.mem()); break;
	case 0x32: CpuInstr::lddMemReg8(*this, emu.mem(), REG16_HL, REG8_A); break;
	case 0x33:
		instrIncReg16(REG16_SP);
		break;
	case 0x36: CpuInstr::ldMemImm(*this, emu.mem(), REG16_HL); break;
	case 0x38:
		instrJr();
		break;
	case 0x39:
		instrAddHlReg16(REG16_SP);
		break;
	case 0x3A: CpuInstr::lddReg8Mem(*this, REG8_A, emu.mem(), REG16_HL); break;
	case 0x3B:
		instrDecReg16(REG16_SP);
		break;
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
	case 0xC0:
		instrRet();
		break;
    case 0xC1: CpuInstr::pop(*this, emu.mem(), REG16_BC); break;
	case 0xC4:
		instrCall();
		break;
    case 0xC5: CpuInstr::push(*this, emu.mem(), REG16_BC); break;
    case 0xC6: CpuInstr::addImm(*this, emu.mem()); break;
	case 0xC8:
		instrRet();
		break;
	case 0xC9:
		instrRet();
		break;
	case 0xCB:
		instrCbMain();
		break;
	case 0xCC:
		instrCall();
		break;
	case 0xCD:
		instrCall();
		break;
	case 0xCE: CpuInstr::addImm(*this, emu.mem(), true); break;
	case 0xD0:
		instrRet();
		break;
	case 0xD1: CpuInstr::pop(*this, emu.mem(), REG16_DE); break;
	case 0xD4:
		instrCall();
		break;
	case 0xD5: CpuInstr::push(*this, emu.mem(), REG16_DE); break;
    case 0xD6: CpuInstr::subImm(*this, emu.mem()); break;
	case 0xD8:
		instrRet();
		break;
	case 0xDC:
		instrCall();
		break;
	case 0xDE: CpuInstr::subImm(*this, emu.mem(), true); break;
    case 0xE0: CpuInstr::ldFFnA(*this, emu.mem()); break;
	case 0xE1: CpuInstr::pop(*this, emu.mem(), REG16_HL); break;
    case 0xE2: CpuInstr::ldFFcA(*this, emu.mem()); break;
	case 0xE5: CpuInstr::push(*this, emu.mem(), REG16_HL); break;
	case 0xE8:
		instrAddSpImm();
		break;
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

/**************************************************************************************************/
/* 16-bit ALU                                                                                     */
/**************************************************************************************************/

//..................................................................................................
void Cpu::instrIncReg16(Reg16 reg)
{
	m_instrCycles = 8;
	setReg16(reg, reg16(reg) + 1);
}

//..................................................................................................
void Cpu::instrDecReg16(Reg16 reg)
{
	m_instrCycles = 8;
	setReg16(reg, reg16(reg) - 1);
}

//..................................................................................................
void Cpu::instrAddHlReg16(Reg16 reg)
{
	m_instrCycles = 8;

	uint16_t hlVal = reg16(REG16_HL);
	uint16_t argVal = reg16(reg);
	setReg16(REG16_HL, hlVal + argVal);
	setFlagN(false);
	// Flags H & C TODO
}

//..................................................................................................
void Cpu::instrAddSpImm(void)
{
	m_instrCycles = 16;
	fetchParam8();

	uint16_t spVal = reg16(REG16_SP);
	uint16_t immVal = m_parameters[0];
	setReg16(REG16_SP, spVal + immVal);
	setFlagZ(false);
	setFlagN(false);
	// Flags H & C TODO
}

/**************************************************************************************************/
/* Jumps                                                                                          */
/**************************************************************************************************/

//..................................................................................................
void Cpu::instrJr(void)
{
	// Exception, instrCycles is set at the end
	fetchParam8();

	bool doJump = false;
	switch (m_opcode)
	{
	case 0x18:
		doJump = true;
		break;
	case 0x20:
		doJump = !flagZ();
		break;
	case 0x28:
		doJump = flagZ();
		break;
	case 0x30:
		doJump = !flagC();
		break;
	case 0x38:
		doJump = flagC();
		break;
	default:
		LOG_WARN("Cpu: Invalid jump instruction");
		break;
	}

	if (doJump)
	{
		m_regPC += (int8_t)m_parameters[0];
		m_instrCycles = 12;
	}
	else
	{
		m_instrCycles = 8;
	}
}

//..................................................................................................
void Cpu::instrCall(void)
{
	// Exception, instrCycles is set at the end
	fetchParam16();

	bool doJump = false;
	switch (m_opcode)
	{
	case 0xC4:
		doJump = !flagZ();
		break;
	case 0xCC:
		doJump = flagZ();
		break;
	case 0xCD:
		doJump = true;
		break;
	case 0xD4:
		doJump = !flagC();
		break;
	case 0xDC:
		doJump = flagC();
		break;
	default:
		LOG_WARN("Cpu: Invalid call instruction");
		break;
	}

	if (doJump)
	{
		instrPush(REG16_PC);
		m_instrCycles = 24;
		m_regPC = m_parameters[0] + (m_parameters[1] << 8);
	}
	else
	{
		m_instrCycles = 12;
	}
}

//..................................................................................................
void Cpu::instrRet(void)
{
	// Exception, instrCycles is set at the end

	bool doRet = false;
	switch (m_opcode)
	{
	case 0xC0:
		doRet = !flagZ();
		m_instrCycles = 4;
		break;
	case 0xC8:
		doRet = flagZ();
		m_instrCycles = 4;
		break;
	case 0xC9:
		doRet = true;
		m_instrCycles = 0;
		break;
	case 0xD0:
		doRet = !flagC();
		m_instrCycles = 4;
		break;
	case 0xD8:
		doRet = flagC();
		m_instrCycles = 4;
		break;
	default:
		LOG_WARN("Cpu: Invalid ret instruction");
		break;
	}

	if (doRet)
	{
		m_instrCycles += 16;

		// POP
		uint16_t spVal = reg16(REG16_SP);
		uint8_t low = m_emu->mem()->read(spVal);
		uint8_t high = m_emu->mem()->read(spVal + 1);
		setReg16(REG16_PC, ((uint16_t)high << 8) + (uint16_t)low);
		setReg16(REG16_SP, spVal + 2);
	}
	else
	{
		m_instrCycles = 8;
	}
}

/**************************************************************************************************/
/* Rotates & Shifts																				  */
/**************************************************************************************************/

//..................................................................................................
void Cpu::instrRlA(bool rlc)
{
	m_instrCycles = 4;
	instrHelperRl(REG8_A, rlc);
}

//..................................................................................................
void Cpu::instrRrA(bool rrc)
{
	m_instrCycles = 4;
	instrHelperRr(REG8_A, rrc);
}

/**************************************************************************************************/
/* CB Extended Instruction																		  */
/**************************************************************************************************/

void Cpu::instrCbMain(void)
{
	fetchParam8();

	uint8_t reg = m_parameters[0] & 0x07;
	uint8_t bitCode = (m_parameters[0] & 0x38) >> 3;
	uint8_t subCode = (m_parameters[0] & 0xC0) >> 6;

	if (subCode == 0)
	{
		switch (bitCode)
		{
		// RLC
		case 0:
			if (reg != 6)
			{
				instrCbRlReg8(static_cast<Reg8>(reg), true);
			}
			else
			{
				instrCbRlMem(true);
			}
			break;

		// RRC
		case 1:
			if (reg != 6)
			{
				instrCbRrReg8(static_cast<Reg8>(reg), true);
			}
			else
			{
				instrCbRrMem(true);
			}
			break;

		// RL
		case 2:
			if (reg != 6)
			{
				instrCbRlReg8(static_cast<Reg8>(reg));
			}
			else
			{
				instrCbRlMem();
			}
			break;

		// RR
		case 3:
			if (reg != 6)
			{
				instrCbRrReg8(static_cast<Reg8>(reg));
			}
			else
			{
				instrCbRrMem();
			}
			break;

		default:
			LOG_WARN("CPU: Unknown CB extended instruction");
		}
	}
	// BIT
	else if (subCode == 1)
	{
		if (reg != 6)
		{
			instrCbBitReg8(static_cast<Reg8>(reg), bitCode);
		}
		else
		{
			instrCbBitMem8(bitCode);
		}
	}
	// RES
	else if (subCode == 2)
	{
		if (reg != 6)
		{
			instrCbResReg8(static_cast<Reg8>(reg), bitCode);
		}
		else
		{
			instrCbResMem8(bitCode);
		}
	}
	// SET
	else if (subCode == 3)
	{
		if (reg != 6)
		{
			instrCbSetReg8(static_cast<Reg8>(reg), bitCode);
		}
		else
		{
			instrCbSetMem8(bitCode);
		}
	}
	else
	{
		LOG_WARN("CPU: Internal error");
		// TODO
	}
}

void Cpu::instrCbBitReg8(Reg8 reg, uint8_t index)
{
	uint8_t mask = 1 << index;
	if ((m_reg8[reg] & mask) == mask)
	{
		setFlagZ(false);
	}
	else
	{
		setFlagZ(true);
	}
	setFlagN(false);
	setFlagH(true);
	m_instrCycles = 8;
}

void Cpu::instrCbBitMem8(uint8_t index)
{
	uint8_t mask = 1 << index;
	if ((m_emu->mem()->read(reg16(REG16_HL)) & mask) == mask)
	{
		setFlagZ(false);
	}
	else
	{
		setFlagZ(true);
	}
	setFlagN(false);
	setFlagH(true);
	m_instrCycles = 16;
}

void Cpu::instrCbResReg8(Reg8 reg, uint8_t index)
{
	uint8_t mask = !(1 << index);
	m_reg8[reg] &= mask;
	m_instrCycles = 8;
}

void Cpu::instrCbResMem8(uint8_t index)
{
	uint8_t mask = !(1 << index);
	uint8_t val = m_emu->mem()->read(reg16(REG16_HL));
	val &= mask;
	m_emu->mem()->write(reg16(REG16_HL), val);
	m_instrCycles = 16;
}

void Cpu::instrCbSetReg8(Reg8 reg, uint8_t index)
{
	uint8_t mask = 1 << index;
	m_reg8[reg] |= mask;
	m_instrCycles = 8;
}

void Cpu::instrCbSetMem8(uint8_t index)
{
	uint8_t mask = 1 << index;
	uint8_t val = m_emu->mem()->read(reg16(REG16_HL));
	val |= mask;
	m_emu->mem()->write(reg16(REG16_HL), val);
	m_instrCycles = 16;
}

void Cpu::instrCbRlReg8(Reg8 reg, bool rlc)
{
	m_instrCycles = 8;
	instrHelperRl(reg, rlc);
}

void Cpu::instrCbRlMem(bool rlc)
{
	m_instrCycles = 16;

	uint8_t memVal = m_emu->mem()->read(reg16(REG16_HL));
	uint8_t oldCarryVal = FLAG_TO_UINT(flagC());
	if ((memVal & 0x80) == 0x80)
	{
		setFlagC(true);
	}
	else
	{
		setFlagC(false);
	}
	uint8_t arg;
	if (rlc)
	{
		// Get the new flag
		arg = FLAG_TO_UINT(flagC());
	}
	else
	{
		arg = oldCarryVal;
	}
	uint8_t newVal = (memVal << 1) + arg;
	m_emu->mem()->write(reg16(REG16_HL), newVal);

	setFlagZ(IS_ZERO(newVal));
	setFlagN(false);
	setFlagH(false);
}

void Cpu::instrCbRrReg8(Reg8 reg, bool rrc)
{
	m_instrCycles = 8;
	instrHelperRr(reg, rrc);
}

void Cpu::instrCbRrMem(bool rrc)
{
	m_instrCycles = 16;

	uint8_t memVal = m_emu->mem()->read(reg16(REG16_HL));
	uint8_t oldCarryVal = FLAG_TO_UINT(flagC());
	if ((memVal & 0x01) == 0x01)
	{
		setFlagC(true);
	}
	else
	{
		setFlagC(false);
	}
	uint8_t arg;
	if (rrc)
	{
		// Get the new flag
		arg = FLAG_TO_UINT(flagC());
	}
	else
	{
		arg = oldCarryVal;
	}
	uint8_t newVal = (memVal >> 1) + (arg << 7);
	m_emu->mem()->write(reg16(REG16_HL), newVal);

	setFlagZ(IS_ZERO(newVal));
	setFlagN(false);
	setFlagH(false);
}

/**************************************************************************************************/
/* Helper functions																				  */
/**************************************************************************************************/

//..................................................................................................
void Cpu::instrHelperRl(Reg8 reg, bool rlc = false)
{
	uint8_t regVal = reg8(reg);
	uint8_t oldCarryVal = FLAG_TO_UINT(flagC());
	if ((regVal & 0x80) == 0x80)
	{
		setFlagC(true);
	}
	else
	{
		setFlagC(false);
	}
	uint8_t arg;
	if (rlc)
	{
		// Get the new flag
		arg = FLAG_TO_UINT(flagC());
	}
	else
	{
		arg = oldCarryVal;
	}
	setReg8(reg, (regVal << 1) + arg);

	setFlagZ(IS_ZERO(reg8(reg)));
	setFlagN(false);
	setFlagH(false);
}

//..................................................................................................
void Cpu::instrHelperRr(Reg8 reg, bool rrc = false)
{
	uint8_t regVal = reg8(reg);
	uint8_t oldCarryVal = FLAG_TO_UINT(flagC());
	if ((regVal & 0x01) == 0x01)
	{
		setFlagC(true);
	}
	else
	{
		setFlagC(false);
	}
	uint8_t arg;
	if (rrc)
	{
		// Get the new flag
		arg = FLAG_TO_UINT(flagC());
	}
	else
	{
		arg = oldCarryVal;
	}
	setReg8(reg, (regVal >> 1) + (arg << 7));

	setFlagZ(IS_ZERO(reg8(reg)));
	setFlagN(false);
	setFlagH(false);
}