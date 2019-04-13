#ifndef LIBDMG_LCD_CONTROLLER_HPP
#define LIBDMG_LCD_CONTROLLER_HPP

#include <cereal/archives/xml.hpp>

namespace LibDMG
{
class Peripherals;

class LcdController
{
  public:
    LcdController() :   m_cycles(0),
                        m_state(STATE_MODE0),
                        m_duration(MODE0_DURATION),
                        m_regLY(0)
    {
    }

    void step(int cyles);

    void saveState(std::ostream &out) { cereal::XMLOutputArchive ar(out); serialize(ar); }
    void loadState(std::istream &in) { cereal::XMLInputArchive ar(in); serialize(ar); }
    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(m_cycles),
           CEREAL_NVP(m_duration),
           CEREAL_NVP(m_state),
           CEREAL_NVP(m_regLY));
    }

    void setRegLCDC(uint8_t val) {}
    void setRegSTAT(uint8_t val) {}
    void setRegSCY(uint8_t val) {}
    void setRegSCX(uint8_t val) {}
    void setRegLY(uint8_t val) {}
    void setRegLYC(uint8_t val) {}
    void setRegDMA(uint8_t val) {}
    void setRegBGP(uint8_t val) {}
    void setRegOBP0(uint8_t val) {}
    void setRegOBP1(uint8_t val) {}
    void setRegWY(uint8_t val) {}
    void setRegWX(uint8_t val) {}

    uint8_t regLCDC(void) const { return 0; }
    uint8_t regSTAT(void) const { return 0; }
    uint8_t regSCY(void) const { return 0; }
    uint8_t regSCX(void) const { return 0; }
    uint8_t regLY(void) const { return m_regLY; }
    uint8_t regLYC(void) const { return 0; }
    uint8_t regDMA(void) const { return 0; }
    uint8_t regBGP(void) const { return 0; }
    uint8_t regOBP0(void) const { return 0; }
    uint8_t regOBP1(void) const { return 0; }
    uint8_t regWY(void) const { return 0; }
    uint8_t regWX(void) const { return 0; }

  private:
    enum state_t
    {
        STATE_MODE0,
        STATE_MODE1,
        STATE_MODE2,
        STATE_MODE3
    };
    static const int MODE0_DURATION = 204;
    static const int MODE1_DURATION = 456; // Mode 1 really lasts 10x that
    static const int MODE2_DURATION = 80;
    static const int MODE3_DURATION = 172;

    int m_cycles;
    int m_duration;
    state_t m_state;

    uint8_t m_regLY;

    void gotoNextState(void);
    void gotoMode0(void) { m_state = STATE_MODE0; m_duration = MODE0_DURATION; }
    void gotoMode1(void) { m_state = STATE_MODE1; m_duration = MODE1_DURATION; }
    void gotoMode2(void) { m_state = STATE_MODE2; m_duration = MODE2_DURATION; }
    void gotoMode3(void) { m_state = STATE_MODE3; m_duration = MODE3_DURATION; }
};
} // namespace LibDMG

#endif // LIBDMG_LCD_CONTROLLER_HPP