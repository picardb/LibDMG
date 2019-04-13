#ifndef LIBDMG_EMULATOR_HPP
#define LIBDMG_EMULATOR_HPP

#include <exception>
#include <memory>
#include <cereal/archives/xml.hpp>
#include <cereal/types/memory.hpp>

#include "cpu/cpu.hpp"
#include "peripherals/peripherals.hpp"
#include "mem/mem_controller_rom_only.hpp"

namespace LibDMG
{
    class Emulator
    {
    public:
        Emulator();

        void step(int cycles);

        Cpu * const cpu() const { return m_cpu.get(); }
        Peripherals * const periph() const { return m_periph.get(); }
        MemControllerBase& mem() const { return *m_mem.get(); }

        void saveState(std::ostream &out) { cereal::XMLOutputArchive ar(out); serialize(ar); }
        void loadState(std::istream &in) { cereal::XMLInputArchive ar(in); serialize(ar); }
        template <class Archive>
        void serialize(Archive &ar)
        {
            ar(CEREAL_NVP(m_cpu),
            CEREAL_NVP(m_periph));
        }

    private:
        std::unique_ptr<Cpu>         m_cpu;
        std::unique_ptr<Peripherals> m_periph;
        std::unique_ptr<MemControllerBase> m_mem;
    };
}

#endif // LIBDMG_EMULATOR_HPP