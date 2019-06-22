#include "cpu_instr.hpp"

#include "logger.hpp"

namespace LibDMG
{
    /**************************************************************************************************/
    /* Jumps                                                                                          */
    /**************************************************************************************************/

    //..................................................................................................
    void CpuInstr::jr(Cpu& cpu, MemControllerBase& mem)
    {
        // Exception, instrCycles is set at the end
        fetchParam8(cpu, mem);

        bool doJump = false;
        switch (cpu.m_opcode)
        {
        case 0x18:
            doJump = true;
            break;
        case 0x20:
            doJump = !cpu.flagZ();
            break;
        case 0x28:
            doJump = cpu.flagZ();
            break;
        case 0x30:
            doJump = !cpu.flagC();
            break;
        case 0x38:
            doJump = cpu.flagC();
            break;
        default:
            LOG_WARN("Cpu: Invalid jump instruction");
            break;
        }

        if (doJump)
        {
            cpu.m_regPC += (int8_t)cpu.m_parameters[0];
            cpu.m_instrCycles = 12;
        }
        else
        {
            cpu.m_instrCycles = 8;
        }
    }

    //..................................................................................................
    void CpuInstr::call(Cpu& cpu, MemControllerBase& mem)
    {
        // Exception, instrCycles is set at the end
        fetchParam16(cpu, mem);

        bool doJump = false;
        switch (cpu.m_opcode)
        {
        case 0xC4:
            doJump = !cpu.flagZ();
            break;
        case 0xCC:
            doJump = cpu.flagZ();
            break;
        case 0xCD:
            doJump = true;
            break;
        case 0xD4:
            doJump = !cpu.flagC();
            break;
        case 0xDC:
            doJump = cpu.flagC();
            break;
        default:
            LOG_WARN("Cpu: Invalid call instruction");
            break;
        }

        if (doJump)
        {
            push(cpu, mem, Cpu::REG16_PC);
            cpu.m_instrCycles = 24;
            cpu.m_regPC = cpu.m_parameters[0] + (cpu.m_parameters[1] << 8);
        }
        else
        {
            cpu.m_instrCycles = 12;
        }

    }

    //..................................................................................................
    void CpuInstr::ret(Cpu& cpu, MemControllerBase& mem)
    {
        // Exception, instrCycles is set at the end

        bool doRet = false;
        switch (cpu.m_opcode)
        {
        case 0xC0:
            doRet = !cpu.flagZ();
            cpu.m_instrCycles = 4;
            break;
        case 0xC8:
            doRet = cpu.flagZ();
            cpu.m_instrCycles = 4;
            break;
        case 0xC9:
            doRet = true;
            cpu.m_instrCycles = 0;
            break;
        case 0xD0:
            doRet = !cpu.flagC();
            cpu.m_instrCycles = 4;
            break;
        case 0xD8:
            doRet = cpu.flagC();
            cpu.m_instrCycles = 4;
            break;
        default:
            LOG_WARN("Cpu: Invalid ret instruction");
            break;
        }

        if (doRet)
        {
            cpu.m_instrCycles += 16;

            // POP
            uint16_t spVal = cpu.reg16(Cpu::REG16_SP);
            uint8_t low = mem.read(spVal);
            uint8_t high = mem.read(spVal + 1);
            cpu.setReg16(Cpu::REG16_PC, ((uint16_t)high << 8) + (uint16_t)low);
            cpu.setReg16(Cpu::REG16_SP, spVal + 2);
        }
        else
        {
            cpu.m_instrCycles = 8;
        }

    }
}