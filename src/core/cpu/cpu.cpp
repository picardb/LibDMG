#include "cpu.hpp"

#include <algorithm>

#include "emulator.hpp"
#include "cpu_instr.hpp"
#include "logger.hpp"

using namespace LibDMG;
using namespace std;

// HELPER MACROS
#define IS_ZERO(a) ((a) == 0 ? (true) : (false))
#define IS_CARRY2(a, b) ((uint16_t)(a) + (uint16_t)(b) > 256 ? (true) : (false))
#define IS_CARRY3(a, b, c) ((uint16_t)(a) + (uint16_t)(b) + (uint16_t)(c) > 256 ? (true) : (false))
#define IS_HALF_CARRY2(a, b) (((a)&0x0F) + ((b)&0x0F) > 16 ? (true) : (false))
#define IS_HALF_CARRY3(a, b, c) (((a)&0x0F) + ((b)&0x0F) + ((c)&0x0F) > 16 ? (true) : (false))
#define IS_BORROW2(a, b) ((a) < (b) ? (true) : (false))
#define IS_BORROW3(a, b, c) ((uint16_t)(a) < (uint16_t)(b) + (uint16_t)(c) ? (true) : (false))
#define IS_HALF_BORROW2(a, b) (((a)&0x0F) < ((b)&0x0F) ? (true) : (false))
#define IS_HALF_BORROW3(a, b, c) (((a)&0x0F) < ((b)&0x0F) + ((c)&0x0F) ? (true) : (false))
#define FLAG_TO_UINT(a) ((a) ? (1) : (0))

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
	case 0x01:
		instrLdReg16Imm(REG16_BC);
		break;
	case 0x02:
		instrLdMemReg8(REG16_BC, REG8_A);
		break;
	case 0x03:
		instrIncReg16(REG16_BC);
		break;
	case 0x04:
		instrIncReg8(REG8_B);
		break;
	case 0x05:
		instrDecReg8(REG8_B);
		break;
	case 0x06: CpuInstr::ldReg8Imm(*this, emu.mem(), REG8_B); break;
	case 0x07:
		instrRlA(true);
		break;
	case 0x08:
		instrLdMemImmSp();
		break;
	case 0x09:
		instrAddHlReg16(REG16_BC);
		break;
	case 0x0A:
		instrLdReg8Mem(REG8_A, REG16_BC);
		break;
	case 0x0B:
		instrDecReg16(REG16_BC);
		break;
	case 0x0C:
		instrIncReg8(REG8_C);
		break;
	case 0x0D:
		instrDecReg8(REG8_C);
		break;
	case 0x0E: CpuInstr::ldReg8Imm(*this, emu.mem(), REG8_C); break;
	case 0x0F:
		instrRrA(true);
		break;
	case 0x11:
		instrLdReg16Imm(REG16_DE);
		break;
	case 0x12:
		instrLdMemReg8(REG16_DE, REG8_A);
		break;
	case 0x13:
		instrIncReg16(REG16_DE);
		break;
	case 0x14:
		instrIncReg8(REG8_D);
		break;
	case 0x15:
		instrDecReg8(REG8_D);
		break;
	case 0x16: CpuInstr::ldReg8Imm(*this, emu.mem(), REG8_D); break;
	case 0x17:
		instrRlA(false);
		break;
	case 0x18:
		instrJr();
		break;
	case 0x19:
		instrAddHlReg16(REG16_DE);
		break;
	case 0x1A:
		instrLdReg8Mem(REG8_A, REG16_DE);
		break;
	case 0x1B:
		instrDecReg16(REG16_DE);
		break;
	case 0x1C:
		instrIncReg8(REG8_E);
		break;
	case 0x1D:
		instrDecReg8(REG8_E);
		break;
	case 0x1E: CpuInstr::ldReg8Imm(*this, emu.mem(), REG8_E); break;
	case 0x1F:
		instrRrA(false);
		break;
	case 0x20:
		instrJr();
		break;
	case 0x21:
		instrLdReg16Imm(REG16_HL);
		break;
	case 0x22:
		instrLdiMemReg8(REG16_HL, REG8_A);
		break;
	case 0x23:
		instrIncReg16(REG16_HL);
		break;
	case 0x24:
		instrIncReg8(REG8_H);
		break;
	case 0x25:
		instrDecReg8(REG8_H);
		break;
	case 0x26: CpuInstr::ldReg8Imm(*this, emu.mem(), REG8_H); break;
	case 0x28:
		instrJr();
		break;
	case 0x29:
		instrAddHlReg16(REG16_HL);
		break;
	case 0x2A:
		instrLdiReg8Mem(REG8_A, REG16_HL);
		break;
	case 0x2B:
		instrDecReg16(REG16_HL);
		break;
	case 0x2C:
		instrIncReg8(REG8_L);
		break;
	case 0x2D:
		instrDecReg8(REG8_L);
		break;
	case 0x2E: CpuInstr::ldReg8Imm(*this, emu.mem(), REG8_L); break;
	case 0x30:
		instrJr();
		break;
	case 0x31:
		instrLdReg16Imm(REG16_SP);
		break;
	case 0x32:
		instrLddMemReg8(REG16_HL, REG8_A);
		break;
	case 0x33:
		instrIncReg16(REG16_SP);
		break;
	case 0x36:
		instrLdMemImm(REG16_HL);
		break;
	case 0x38:
		instrJr();
		break;
	case 0x39:
		instrAddHlReg16(REG16_SP);
		break;
	case 0x3A:
		instrLddReg8Mem(REG8_A, REG16_HL);
		break;
	case 0x3B:
		instrDecReg16(REG16_SP);
		break;
	case 0x3C:
		instrIncReg8(REG8_A);
		break;
	case 0x3D:
		instrDecReg8(REG8_A);
		break;
	case 0x3E: CpuInstr::ldReg8Imm(*this, REG8_A, emu.mem()); break;
	case 0x40: CpuInstr::ldReg8Reg8(*this, REG8_B, REG8_B); break;
	case 0x41: CpuInstr::ldReg8Reg8(*this, REG8_B, REG8_C); break;
	case 0x42: CpuInstr::ldReg8Reg8(*this, REG8_B, REG8_D); break;
	case 0x43: CpuInstr::ldReg8Reg8(*this, REG8_B, REG8_E); break;
    case 0x44: CpuInstr::ldReg8Reg8(*this, REG8_B, REG8_H); break;
    case 0x45: CpuInstr::ldReg8Reg8(*this, REG8_B, REG8_L); break;
	case 0x46:
		instrLdReg8Mem(REG8_B, REG16_HL);
		break;
    case 0x47: CpuInstr::ldReg8Reg8(*this, REG8_B, REG8_A); break;
    case 0x48: CpuInstr::ldReg8Reg8(*this, REG8_C, REG8_B); break;
    case 0x49: CpuInstr::ldReg8Reg8(*this, REG8_C, REG8_C); break;
	case 0x4A:
		instrLdReg8Reg8(REG8_C, REG8_D);
		break;
	case 0x4B:
		instrLdReg8Reg8(REG8_C, REG8_E);
		break;
	case 0x4C:
		instrLdReg8Reg8(REG8_C, REG8_H);
		break;
	case 0x4D:
		instrLdReg8Reg8(REG8_C, REG8_L);
		break;
	case 0x4E:
		instrLdReg8Mem(REG8_C, REG16_HL);
		break;
	case 0x4F:
		instrLdReg8Reg8(REG8_C, REG8_A);
		break;
	case 0x50:
		instrLdReg8Reg8(REG8_D, REG8_B);
		break;
	case 0x51:
		instrLdReg8Reg8(REG8_D, REG8_C);
		break;
	case 0x52:
		instrLdReg8Reg8(REG8_D, REG8_D);
		break;
	case 0x53:
		instrLdReg8Reg8(REG8_D, REG8_E);
		break;
	case 0x54:
		instrLdReg8Reg8(REG8_D, REG8_H);
		break;
	case 0x55:
		instrLdReg8Reg8(REG8_D, REG8_L);
		break;
	case 0x56:
		instrLdReg8Mem(REG8_D, REG16_HL);
		break;
	case 0x57:
		instrLdReg8Reg8(REG8_D, REG8_A);
		break;
	case 0x58:
		instrLdReg8Reg8(REG8_E, REG8_B);
		break;
	case 0x59:
		instrLdReg8Reg8(REG8_E, REG8_C);
		break;
	case 0x5A:
		instrLdReg8Reg8(REG8_E, REG8_D);
		break;
	case 0x5B:
		instrLdReg8Reg8(REG8_E, REG8_E);
		break;
	case 0x5C:
		instrLdReg8Reg8(REG8_E, REG8_H);
		break;
	case 0x5D:
		instrLdReg8Reg8(REG8_E, REG8_L);
		break;
	case 0x5E:
		instrLdReg8Mem(REG8_E, REG16_HL);
		break;
	case 0x5F:
		instrLdReg8Reg8(REG8_E, REG8_A);
		break;
	case 0x60:
		instrLdReg8Reg8(REG8_H, REG8_B);
		break;
	case 0x61:
		instrLdReg8Reg8(REG8_H, REG8_C);
		break;
	case 0x62:
		instrLdReg8Reg8(REG8_H, REG8_D);
		break;
	case 0x63:
		instrLdReg8Reg8(REG8_H, REG8_E);
		break;
	case 0x64:
		instrLdReg8Reg8(REG8_H, REG8_H);
		break;
	case 0x65:
		instrLdReg8Reg8(REG8_H, REG8_L);
		break;
	case 0x66:
		instrLdReg8Mem(REG8_H, REG16_HL);
		break;
	case 0x67:
		instrLdReg8Reg8(REG8_H, REG8_A);
		break;
	case 0x68:
		instrLdReg8Reg8(REG8_L, REG8_B);
		break;
	case 0x69:
		instrLdReg8Reg8(REG8_L, REG8_C);
		break;
	case 0x6A:
		instrLdReg8Reg8(REG8_L, REG8_D);
		break;
	case 0x6B:
		instrLdReg8Reg8(REG8_L, REG8_E);
		break;
	case 0x6C:
		instrLdReg8Reg8(REG8_L, REG8_H);
		break;
	case 0x6D:
		instrLdReg8Reg8(REG8_L, REG8_L);
		break;
	case 0x6E:
		instrLdReg8Mem(REG8_L, REG16_HL);
		break;
	case 0x6F:
		instrLdReg8Reg8(REG8_L, REG8_A);
		break;
	case 0x70:
		instrLdMemReg8(REG16_HL, REG8_B);
		break;
	case 0x71:
		instrLdMemReg8(REG16_HL, REG8_C);
		break;
	case 0x72:
		instrLdMemReg8(REG16_HL, REG8_D);
		break;
	case 0x73:
		instrLdMemReg8(REG16_HL, REG8_E);
		break;
	case 0x74:
		instrLdMemReg8(REG16_HL, REG8_H);
		break;
	case 0x75:
		instrLdMemReg8(REG16_HL, REG8_L);
		break;
	case 0x77:
		instrLdMemReg8(REG16_HL, REG8_A);
		break;
	case 0x78:
		instrLdReg8Reg8(REG8_A, REG8_B);
		break;
	case 0x79:
		instrLdReg8Reg8(REG8_A, REG8_C);
		break;
	case 0x7A:
		instrLdReg8Reg8(REG8_A, REG8_D);
		break;
	case 0x7B:
		instrLdReg8Reg8(REG8_A, REG8_E);
		break;
	case 0x7C:
		instrLdReg8Reg8(REG8_A, REG8_H);
		break;
	case 0x7D:
		instrLdReg8Reg8(REG8_A, REG8_L);
		break;
	case 0x7E:
		instrLdReg8Mem(REG8_A, REG16_HL);
		break;
	case 0x7F:
		instrLdReg8Reg8(REG8_A, REG8_A);
		break;
	case 0x80:
		instrAddReg8(REG8_B);
		break;
	case 0x81:
		instrAddReg8(REG8_C);
		break;
	case 0x82:
		instrAddReg8(REG8_D);
		break;
	case 0x83:
		instrAddReg8(REG8_E);
		break;
	case 0x84:
		instrAddReg8(REG8_H);
		break;
	case 0x85:
		instrAddReg8(REG8_L);
		break;
	case 0x86:
		instrAddMem(REG16_HL);
		break;
	case 0x87:
		instrAddReg8(REG8_A);
		break;
	case 0x88:
		instrAddReg8(REG8_B, true);
		break;
	case 0x89:
		instrAddReg8(REG8_C, true);
		break;
	case 0x8A:
		instrAddReg8(REG8_D, true);
		break;
	case 0x8B:
		instrAddReg8(REG8_E, true);
		break;
	case 0x8C:
		instrAddReg8(REG8_H, true);
		break;
	case 0x8D:
		instrAddReg8(REG8_L, true);
		break;
	case 0x8E:
		instrAddMem(REG16_HL, true);
		break;
	case 0x8F:
		instrAddReg8(REG8_A, true);
		break;
	case 0x90:
		instrSubReg8(REG8_B);
		break;
	case 0x91:
		instrSubReg8(REG8_C);
		break;
	case 0x92:
		instrSubReg8(REG8_D);
		break;
	case 0x93:
		instrSubReg8(REG8_E);
		break;
	case 0x94:
		instrSubReg8(REG8_H);
		break;
	case 0x95:
		instrSubReg8(REG8_L);
		break;
	case 0x96:
		instrSubMem(REG16_HL);
		break;
	case 0x97:
		instrSubReg8(REG8_A);
		break;
	case 0x98:
		instrSubReg8(REG8_B, true);
		break;
	case 0x99:
		instrSubReg8(REG8_C, true);
		break;
	case 0x9A:
		instrSubReg8(REG8_D, true);
		break;
	case 0x9B:
		instrSubReg8(REG8_E, true);
		break;
	case 0x9C:
		instrSubReg8(REG8_H, true);
		break;
	case 0x9D:
		instrSubReg8(REG8_L, true);
		break;
	case 0x9E:
		instrSubMem(REG16_HL, true);
		break;
	case 0x9F:
		instrSubReg8(REG8_A, true);
		break;
	case 0xA8:
		instrXorReg8(REG8_B);
		break;
	case 0xA9:
		instrXorReg8(REG8_C);
		break;
	case 0xAA:
		instrXorReg8(REG8_D);
		break;
	case 0xAB:
		instrXorReg8(REG8_E);
		break;
	case 0xAC:
		instrXorReg8(REG8_H);
		break;
	case 0xAD:
		instrXorReg8(REG8_L);
		break;
	case 0xAF:
		instrXorReg8(REG8_A);
		break;
	case 0xB8:
		instrCpReg8(REG8_B);
		break;
	case 0xB9:
		instrCpReg8(REG8_C);
		break;
	case 0xBA:
		instrCpReg8(REG8_D);
		break;
	case 0xBB:
		instrCpReg8(REG8_E);
		break;
	case 0xBC:
		instrCpReg8(REG8_H);
		break;
	case 0xBD:
		instrCpReg8(REG8_L);
		break;
	case 0xBE:
		instrCpMem();
		break;
	case 0xBF:
		instrCpReg8(REG8_A);
		break;
	case 0xC0:
		instrRet();
		break;
	case 0xC1:
		instrPop(REG16_BC);
		break;
	case 0xC4:
		instrCall();
		break;
	case 0xC5:
		instrPush(REG16_BC);
		break;
	case 0xC6:
		instrAddImm();
		break;
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
	case 0xCE:
		instrAddImm(true);
		break;
	case 0xD0:
		instrRet();
		break;
	case 0xD1:
		instrPop(REG16_DE);
		break;
	case 0xD4:
		instrCall();
		break;
	case 0xD5:
		instrPush(REG16_DE);
		break;
	case 0xD6:
		instrSubImm();
		break;
	case 0xD8:
		instrRet();
		break;
	case 0xDC:
		instrCall();
		break;
	case 0xDE:
		instrSubImm(true);
		break;
	case 0xE0:
		instrLdFFnA();
		break;
	case 0xE1:
		instrPop(REG16_HL);
		break;
	case 0xE2:
		instrLdFFcA();
		break;
	case 0xE5:
		instrPush(REG16_HL);
		break;
	case 0xE8:
		instrAddSpImm();
		break;
	case 0xEA:
		instrLdMemImmReg8(REG8_A);
		break;
	case 0xF0:
		instrLdAFFn();
		break;
	case 0xF1:
		instrPop(REG16_AF);
		break;
	case 0xF2:
		instrLdAFFc();
		break;
	case 0xF5:
		instrPush(REG16_AF);
		break;
	case 0xF8:
		instrLdHlSpImm();
		break;
	case 0xF9:
		instrLdSpHl();
		break;
	case 0xFA:
		instrLdReg8MemImm(REG8_A);
		break;
	case 0xFE:
		instrCpImm();
		break;
	default:
		LOG_WARN("CPU: Unknown instruction");
		CpuInstr::nop(*this);
		break;
	}
}

//..................................................................................................
void Cpu::instrLdReg8Mem(Reg8 reg, Reg16 addr)
{
	m_instrCycles = 8;
	setReg8(reg, m_emu->mem()->read(reg16(addr)));
}

//..................................................................................................
void Cpu::instrLddReg8Mem(Reg8 reg, Reg16 addr)
{
	instrLdReg8Mem(reg, addr);
	setReg16(addr, reg16(addr) - 1);
}

//..................................................................................................
void Cpu::instrLdiReg8Mem(Reg8 reg, Reg16 addr)
{
	instrLdReg8Mem(reg, addr);
	setReg16(addr, reg16(addr) + 1);
}

//..................................................................................................
void Cpu::instrLdReg8MemImm(Reg8 reg)
{
	m_instrCycles = 16;
	fetchParam16();
	uint16_t addr = m_parameters[0] + (m_parameters[1] << 8);
	m_reg8[reg] = m_emu->mem()->read(addr);
}

//..................................................................................................
void Cpu::instrLdMemReg8(Reg16 addr, Reg8 reg)
{
	m_instrCycles = 8;
	uint16_t address = reg16(addr);
	m_emu->mem()->write(address, m_reg8[reg]);
}

//..................................................................................................
void Cpu::instrLddMemReg8(Reg16 addr, Reg8 reg)
{
	instrLdMemReg8(addr, reg);
	setReg16(addr, reg16(addr) - 1);
}

//..................................................................................................
void Cpu::instrLdiMemReg8(Reg16 addr, Reg8 reg)
{
	instrLdMemReg8(addr, reg);
	setReg16(addr, reg16(addr) + 1);
}

//..................................................................................................
void Cpu::instrLdMemImm(Reg16 addr)
{
	m_instrCycles = 12;
	fetchParam8();
	m_emu->mem()->write(reg16(addr), m_parameters[0]);
}

//..................................................................................................
void Cpu::instrLdMemImmReg8(Reg8 reg)
{
	m_instrCycles = 16;
	fetchParam16();
	m_emu->mem()->write(m_parameters[0] + (m_parameters[1] << 8), m_reg8[reg]);
}

//..................................................................................................
void Cpu::instrLdAFFc(void)
{
	m_instrCycles = 8;
	m_reg8[REG8_A] = m_emu->mem()->read(0xFF00 + m_reg8[REG8_C]);
}

//..................................................................................................
void Cpu::instrLdFFcA(void)
{
	m_instrCycles = 8;
	m_emu->mem()->write(0xFF00 + m_reg8[REG8_C], m_reg8[REG8_A]);
}

//..................................................................................................
void Cpu::instrLdAFFn(void)
{
	m_instrCycles = 12;
	fetchParam8();
	m_reg8[REG8_A] = m_emu->mem()->read(0xFF00 + m_parameters[0]);
}

//..................................................................................................
void Cpu::instrLdFFnA(void)
{
	m_instrCycles = 12;
	fetchParam8();
	m_emu->mem()->write(0xFF00 + m_parameters[0], m_reg8[REG8_A]);
}

/**************************************************************************************************/
/* 16-bit loads                                                                                   */
/**************************************************************************************************/

//..................................................................................................
void Cpu::instrLdReg16Imm(Reg16 reg)
{
	m_instrCycles = 12;
	fetchParam16();
	setReg16(reg, (m_parameters[1] << 8) + m_parameters[0]);
}

//..................................................................................................
void Cpu::instrLdSpHl(void)
{
	m_instrCycles = 8;
	setReg16(REG16_SP, reg16(REG16_HL));
}

//..................................................................................................
void Cpu::instrLdHlSpImm(void)
{
	m_instrCycles = 12;
	fetchParam8();
	uint16_t result = (uint16_t)((int16_t)reg16(REG16_SP) + (int16_t)m_parameters[0]);
	setReg16(REG16_HL, result);

	setFlagZ(false);
	setFlagN(false);
	setFlagH(IS_HALF_CARRY2(reg16(REG16_SP), m_parameters[0]));
	setFlagC(IS_CARRY2(reg16(REG16_SP), m_parameters[0]));
}

//..................................................................................................
void Cpu::instrLdMemImmSp(void)
{
	m_instrCycles = 20;
	fetchParam16();
	uint16_t address = m_parameters[0] + (m_parameters[1] << 8);
	m_emu->mem()->write(address, (uint8_t)(reg16(REG16_SP) && 0x00FF));
	m_emu->mem()->write(address + 1, (uint8_t)((reg16(REG16_SP) && 0xFF00) >> 8));
}

//..................................................................................................
void Cpu::instrPush(Reg16 reg)
{
	m_instrCycles = 16;
	uint16_t spVal = reg16(REG16_SP);
	m_emu->mem()->write(spVal - 1, (uint8_t)((reg16(reg) & 0xFF00) >> 8));
	m_emu->mem()->write(spVal - 2, (uint8_t)(reg16(reg) & 0x00FF));
	setReg16(REG16_SP, spVal - 2);
}

//..................................................................................................
void Cpu::instrPop(Reg16 reg)
{
	m_instrCycles = 12;
	uint16_t spVal = reg16(REG16_SP);
	uint8_t low = m_emu->mem()->read(spVal);
	uint8_t high = m_emu->mem()->read(spVal + 1);
	setReg16(reg, ((uint16_t)high << 8) + (uint16_t)low);
	setReg16(REG16_SP, spVal + 2);
}

/**************************************************************************************************/
/* 8-bit ALU                                                                                      */
/**************************************************************************************************/

//..................................................................................................
void Cpu::instrAddReg8(Reg8 reg, bool carry)
{
	m_instrCycles = 4;

	uint8_t aVal = m_reg8[REG8_A];
	uint8_t regVal = m_reg8[reg];
	if (carry)
	{
		uint8_t carryVal = FLAG_TO_UINT(flagC());
		m_reg8[REG8_A] += regVal + carryVal;
		setFlagH(IS_HALF_CARRY3(aVal, regVal, carryVal));
		setFlagC(IS_CARRY3(aVal, regVal, carryVal));
	}
	else
	{
		m_reg8[REG8_A] += regVal;
		setFlagH(IS_HALF_CARRY2(aVal, regVal));
		setFlagC(IS_CARRY2(aVal, regVal));
	}
	setFlagZ(IS_ZERO(m_reg8[REG8_A]));
	setFlagN(false);
}

//..................................................................................................
void Cpu::instrAddMem(Reg16 reg, bool carry)
{
	m_instrCycles = 8;

	uint8_t aVal = m_reg8[REG8_A];
	uint8_t memVal = m_emu->mem()->read(reg16(reg));
	if (carry)
	{
		uint8_t carryVal = FLAG_TO_UINT(flagC());
		m_reg8[REG8_A] += memVal + carryVal;
		setFlagH(IS_HALF_CARRY3(aVal, memVal, carryVal));
		setFlagC(IS_CARRY3(aVal, memVal, carryVal));
	}
	else
	{
		m_reg8[REG8_A] += memVal;
		setFlagH(IS_HALF_CARRY2(aVal, memVal));
		setFlagC(IS_CARRY2(aVal, memVal));
	}
	setFlagZ(IS_ZERO(m_reg8[REG8_A]));
	setFlagN(false);
}

//..................................................................................................
void Cpu::instrAddImm(bool carry)
{
	m_instrCycles = 8;
	fetchParam8();

	uint8_t aVal = m_reg8[REG8_A];
	uint8_t immVal = m_parameters[0];
	if (carry)
	{
		uint8_t carryVal = FLAG_TO_UINT(flagC());
		m_reg8[REG8_A] += immVal + carryVal;
		setFlagH(IS_HALF_CARRY3(aVal, immVal, carryVal));
		setFlagC(IS_CARRY3(aVal, immVal, carryVal));
	}
	else
	{
		m_reg8[REG8_A] += immVal;
		setFlagH(IS_HALF_CARRY2(aVal, immVal));
		setFlagC(IS_CARRY2(aVal, immVal));
	}
	setFlagZ(IS_ZERO(m_reg8[REG8_A]));
	setFlagN(false);
}

//..................................................................................................
void Cpu::instrSubReg8(Reg8 reg, bool carry)
{
	m_instrCycles = 4;

	uint8_t aVal = m_reg8[REG8_A];
	uint8_t regVal = m_reg8[reg];
	if (carry)
	{
		uint8_t carryVal = FLAG_TO_UINT(flagC());
		m_reg8[REG8_A] -= (regVal + carryVal);
		setFlagH(IS_HALF_BORROW3(aVal, regVal, carryVal));
		setFlagC(IS_BORROW3(aVal, regVal, carryVal));
	}
	else
	{
		m_reg8[REG8_A] -= regVal;
		setFlagH(IS_HALF_BORROW2(aVal, regVal));
		setFlagC(IS_BORROW2(aVal, regVal));
	}
	setFlagZ(IS_ZERO(m_reg8[REG8_A]));
	setFlagN(true);
}

//..................................................................................................
void Cpu::instrSubMem(Reg16 reg, bool carry)
{
	m_instrCycles = 8;

	uint8_t aVal = m_reg8[REG8_A];
	uint8_t memVal = m_emu->mem()->read(reg16(reg));
	if (carry)
	{
		uint8_t carryVal = FLAG_TO_UINT(flagC());
		m_reg8[REG8_A] -= (memVal + carryVal);
		setFlagH(IS_HALF_BORROW3(aVal, memVal, carryVal));
		setFlagC(IS_BORROW3(aVal, memVal, carryVal));
	}
	else
	{
		m_reg8[REG8_A] -= memVal;
		setFlagH(IS_HALF_BORROW2(aVal, memVal));
		setFlagC(IS_BORROW2(aVal, memVal));
	}
	setFlagZ(IS_ZERO(m_reg8[REG8_A]));
	setFlagN(true);
}

//..................................................................................................
void Cpu::instrSubImm(bool carry)
{
	m_instrCycles = 8;
	fetchParam8();

	uint8_t aVal = m_reg8[REG8_A];
	uint8_t immVal = m_parameters[0];
	if (carry)
	{
		uint8_t carryVal = FLAG_TO_UINT(flagC());
		m_reg8[REG8_A] -= (immVal + carryVal);
		setFlagH(IS_HALF_BORROW3(aVal, immVal, carryVal));
		setFlagC(IS_BORROW3(aVal, immVal, carryVal));
	}
	else
	{
		m_reg8[REG8_A] -= immVal;
		setFlagH(IS_HALF_BORROW2(aVal, immVal));
		setFlagC(IS_BORROW2(aVal, immVal));
	}
	setFlagZ(IS_ZERO(m_reg8[REG8_A]));
	setFlagN(true);
}

//..................................................................................................
void Cpu::instrXorReg8(Reg8 reg)
{
	m_instrCycles = 4;
	m_reg8[REG8_A] ^= m_reg8[reg];
	setFlagZ(IS_ZERO(m_reg8[REG8_A]));
	setFlagN(false);
	setFlagH(false);
	setFlagC(false);
}

//..................................................................................................
void Cpu::instrIncReg8(Reg8 reg)
{
	m_instrCycles = 4;

	uint8_t regVal = reg8(reg);
	setReg8(reg, regVal + 1);
	setFlagZ(IS_ZERO(reg8(reg)));
	setFlagN(false);
	setFlagH(IS_HALF_CARRY2(regVal, 1));
}

//..................................................................................................
void Cpu::instrDecReg8(Reg8 reg)
{
	m_instrCycles = 4;

	uint8_t regVal = reg8(reg);
	setReg8(reg, regVal - 1);
	setFlagZ(IS_ZERO(reg8(reg)));
	setFlagN(true);
	setFlagH(IS_HALF_BORROW2(regVal, 1));
}

//..................................................................................................
void Cpu::instrCpReg8(Reg8 reg)
{
	m_instrCycles = 4;

	uint8_t aVal = reg8(REG8_A);
	uint8_t regVal = reg8(reg);
	setFlagZ(IS_ZERO(aVal - regVal));
	setFlagN(true);
	setFlagH(IS_HALF_BORROW2(aVal, regVal));
	setFlagC(IS_BORROW2(aVal, regVal));
}

//..................................................................................................
void Cpu::instrCpMem(void)
{
	m_instrCycles = 8;

	uint8_t aVal = reg8(REG8_A);
	uint8_t memVal = m_emu->mem()->read(reg16(REG16_HL));
	setFlagZ(IS_ZERO(aVal - memVal));
	setFlagN(true);
	setFlagH(IS_HALF_BORROW2(aVal, memVal));
	setFlagC(IS_BORROW2(aVal, memVal));
}

//..................................................................................................
void Cpu::instrCpImm(void)
{
	m_instrCycles = 8;
	fetchParam8();

	uint8_t aVal = reg8(REG8_A);
	uint8_t immVal = m_parameters[0];
	setFlagZ(IS_ZERO(aVal - immVal));
	setFlagN(true);
	setFlagH(IS_HALF_BORROW2(aVal, immVal));
	setFlagC(IS_BORROW2(aVal, immVal));
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