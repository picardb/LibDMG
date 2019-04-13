#ifndef LIBDMG_TIMER_HPP
#define LIBDMG_TIMER_HPP

#include <cstdint>
#include <exception>
#include <cereal/archives/xml.hpp>

namespace LibDMG
{
    class Peripherals;

    class Timer
    {
    public:
        Timer() :
            m_divPreCounter(0),
            m_timaPreCounter(0),
            m_timaPrescaler(1024),
            m_isRunning(false),
            m_intTIMAPending(false),
            m_regDIV(0),
            m_regTIMA(0),
            m_regTMA(0),
            m_regTAC(0)
        { }

        void step(int cycles);
        void saveState(std::ostream& out) { cereal::XMLOutputArchive ar(out); serialize(ar); }
        void loadState(std::istream& in) { cereal::XMLInputArchive ar(in); serialize(ar); }
        template<class Archive>
        void serialize(Archive & ar)
        {
            ar(CEREAL_NVP(m_divPreCounter),
               CEREAL_NVP(m_timaPreCounter),
               CEREAL_NVP(m_timaPrescaler),
               CEREAL_NVP(m_isRunning),
               CEREAL_NVP(m_intTIMAPending),
               CEREAL_NVP(m_regDIV),
               CEREAL_NVP(m_regTIMA),
               CEREAL_NVP(m_regTMA),
               CEREAL_NVP(m_regTAC));
        }

        void setRegDIV(uint8_t val);
        inline void setRegTIMA(uint8_t val) { m_regTIMA = val; }
        inline void setRegTMA(uint8_t val) { m_regTMA = val; }
        void setRegTAC(uint8_t val);

        uint8_t regDIV() const { return m_regDIV; }
        uint8_t regTIMA() const { return m_regTIMA; }
        uint8_t regTMA() const { return m_regTMA; }
        uint8_t regTAC() const { return m_regTAC; }
        bool intTimaPending() const { return m_intTIMAPending; }
        void clearTIMAPending() { m_intTIMAPending = false; }

    private:
        static const int TIMER_DIV_PRESCALER = 256;
        int m_divPreCounter;
        int m_timaPreCounter;
        int m_timaPrescaler;
        bool m_isRunning;
        bool m_intTIMAPending;

        uint8_t m_regDIV;
        uint8_t m_regTIMA;
        uint8_t m_regTMA;
        uint8_t m_regTAC;       
    };
}

#endif // LIBDMG_TIMER_HPP