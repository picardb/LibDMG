#include "peripherals/timer.hpp"
#include "gtest/gtest.h"

using namespace LibDMG;

namespace {
    class TimerTest : public ::testing::Test {
    protected:
        // You can remove any or all of the following functions if its body
        // is empty.

        TimerTest() {
            // You can do set-up work for each test here.
        }

        ~TimerTest() override {
            // You can do clean-up work that doesn't throw exceptions here.
        }

        // If the constructor and destructor are not enough for setting up
        // and cleaning up each test, you can define the following methods:

        void SetUp() override {
            // Code here will be called immediately after the constructor (right
            // before each test).
        }

        void TearDown() override {
            // Code here will be called immediately after each test (right
            // before the destructor).
        }

        // Objects declared here can be used by all tests in the test case for Foo.
    };

    //
    TEST_F(TimerTest, TimerRunFor100sOff) {
        Timer timer;
        for (int j = 0; j < 100; j++) {
            for (int i = 0; i < 4194304; i++) {
                timer.step(1);
            }
        }
        ASSERT_EQ(timer.regTIMA(), 0);
    }

    //
    TEST_F(TimerTest, TimerRunFor100sOnMachinePrescalerDefault) {
        Timer timer;
        timer.setRegTAC(0x04);
        for (int j = 0; j < 100; j++) {
            for (int i = 0; i < 4194304; i++) {
                timer.step(1);
            }
        }
        ASSERT_EQ(timer.regTIMA(), 0);
    }

    TEST_F(TimerTest, TimerRunFor100sOnMachinePrescaler16) {
        Timer timer;
        timer.setRegTAC(0x05);
        for (int j = 0; j < 100; j++) {
            for (int i = 0; i < 4194304; i++) {
                timer.step(1);
            }
        }
        ASSERT_EQ(timer.regTIMA(), 0);
    }

    TEST_F(TimerTest, TimerRunFor100sOnMachinePrescaler64) {
        Timer timer;
        timer.setRegTAC(0x06);
        for (int j = 0; j < 100; j++) {
            for (int i = 0; i < 4194304; i++) {
                timer.step(1);
            }
        }
        ASSERT_EQ(timer.regTIMA(), 0);
    }

    TEST_F(TimerTest, TimerRunFor100sOnMachinePrescaler256) {
        Timer timer;
        timer.setRegTAC(0x06);
        for (int j = 0; j < 100; j++) {
            for (int i = 0; i < 4194304; i++) {
                timer.step(1);
            }
        }
        ASSERT_EQ(timer.regTIMA(), 0);
    }

    TEST_F(TimerTest, TimerRunFor100sOnInstrPrescalerDefault) {
        Timer timer;
        timer.setRegTAC(0x04);
        for (int j = 0; j < 100; j++) {
            for (int i = 0; i < 1048576; i++) {
                timer.step(4);
            }
        }
        ASSERT_EQ(timer.regTIMA(), 0);
    }

    TEST_F(TimerTest, TimerRunFor100sOnInstrPrescaler16) {
        Timer timer;
        timer.setRegTAC(0x05);
        for (int j = 0; j < 100; j++) {
            for (int i = 0; i < 1048576; i++) {
                timer.step(4);
            }
        }
        ASSERT_EQ(timer.regTIMA(), 0);
    }

    TEST_F(TimerTest, TimerRunFor100sOnInstrPrescaler64) {
        Timer timer;
        timer.setRegTAC(0x06);
        for (int j = 0; j < 100; j++) {
            for (int i = 0; i < 1048576; i++) {
                timer.step(4);
            }
        }
        ASSERT_EQ(timer.regTIMA(), 0);
    }

    TEST_F(TimerTest, TimerRunFor100sOnInstrPrescaler256) {
        Timer timer;
        timer.setRegTAC(0x07);
        for (int j = 0; j < 100; j++) {
            for (int i = 0; i < 1048576; i++) {
                timer.step(4);
            }
        }
        ASSERT_EQ(timer.regTIMA(), 0);
    }

    TEST_F(TimerTest, TimerTestTIMAOff) {
        Timer timer;
        timer.step(1023);
        ASSERT_EQ(timer.regTIMA(), 0);
        timer.step(1);
        ASSERT_EQ(timer.regTIMA(), 0);
        timer.step(1);
        ASSERT_EQ(timer.regTIMA(), 0);
        timer.step(1024 * 254);
        ASSERT_EQ(timer.regTIMA(), 0);
    }

    TEST_F(TimerTest, TimerTestTIMAPrescalerDefault) {
        Timer timer;
        timer.setRegTAC(0x04);
        timer.step(1023);
        ASSERT_EQ(timer.regTIMA(), 0);
        timer.step(1);
        ASSERT_EQ(timer.regTIMA(), 1);
        timer.step(1);
        ASSERT_EQ(timer.regTIMA(), 1);
        timer.step(1022);
        ASSERT_EQ(timer.regTIMA(), 1);
        timer.step(1);
        ASSERT_EQ(timer.regTIMA(), 2);
        timer.step(1024 * 254);
        ASSERT_EQ(timer.regTIMA(), 0);
    }

    TEST_F(TimerTest, TimerTestTIMAPrescaler16) {
        Timer timer;
        timer.setRegTAC(0x05);
        timer.step(15);
        ASSERT_EQ(timer.regTIMA(), 0);
        timer.step(1);
        ASSERT_EQ(timer.regTIMA(), 1);
        timer.step(1);
        ASSERT_EQ(timer.regTIMA(), 1);
        timer.step(14);
        ASSERT_EQ(timer.regTIMA(), 1);
        timer.step(1);
        ASSERT_EQ(timer.regTIMA(), 2);
        timer.step(16 * 254);
        ASSERT_EQ(timer.regTIMA(), 0);
    }

    TEST_F(TimerTest, TimerTestTIMAPrescaler64) {
        Timer timer;
        timer.setRegTAC(0x06);
        timer.step(63);
        ASSERT_EQ(timer.regTIMA(), 0);
        timer.step(1);
        ASSERT_EQ(timer.regTIMA(), 1);
        timer.step(1);
        ASSERT_EQ(timer.regTIMA(), 1);
        timer.step(62);
        ASSERT_EQ(timer.regTIMA(), 1);
        timer.step(1);
        ASSERT_EQ(timer.regTIMA(), 2);
        timer.step(64 * 254);
        ASSERT_EQ(timer.regTIMA(), 0);
    }

    TEST_F(TimerTest, TimerTestTIMAPrescaler256) {
        Timer timer;
        timer.setRegTAC(0x07);
        timer.step(255);
        ASSERT_EQ(timer.regTIMA(), 0);
        timer.step(1);
        ASSERT_EQ(timer.regTIMA(), 1);
        timer.step(1);
        ASSERT_EQ(timer.regTIMA(), 1);
        timer.step(254);
        ASSERT_EQ(timer.regTIMA(), 1);
        timer.step(1);
        ASSERT_EQ(timer.regTIMA(), 2);
        timer.step(256 * 254);
        ASSERT_EQ(timer.regTIMA(), 0);
    }

    TEST_F(TimerTest, TimerTestDIVOff) {
        Timer timer;
        ASSERT_EQ(timer.regDIV(), 0);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 0);
        timer.step(254);
        ASSERT_EQ(timer.regDIV(), 0);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 1);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 1);
        timer.step(255);
        ASSERT_EQ(timer.regDIV(), 2);
        timer.step(100 * 256);
        ASSERT_EQ(timer.regDIV(), 102);
        timer.step(153 * 256 + 255);
        ASSERT_EQ(timer.regDIV(), 255);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 0);
    }

    TEST_F(TimerTest, TimerTestDIVPrescalerPrescalerDefault) {
        Timer timer;
        timer.setRegDIV(18);
        timer.setRegTAC(0x04);
        ASSERT_EQ(timer.regDIV(), 0);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 0);
        timer.step(254);
        ASSERT_EQ(timer.regDIV(), 0);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 1);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 1);
        timer.step(255);
        ASSERT_EQ(timer.regDIV(), 2);
        timer.step(100 * 256);
        ASSERT_EQ(timer.regDIV(), 102);
        timer.setRegDIV(241);
        ASSERT_EQ(timer.regDIV(), 0);
        timer.step(153 * 256 + 255);
        ASSERT_EQ(timer.regDIV(), 153);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 154);
    }

    TEST_F(TimerTest, TimerTestDIVPrescaler16) {
        Timer timer;
        timer.setRegTAC(0x05);
        ASSERT_EQ(timer.regDIV(), 0);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 0);
        timer.step(254);
        ASSERT_EQ(timer.regDIV(), 0);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 1);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 1);
        timer.step(255);
        ASSERT_EQ(timer.regDIV(), 2);
        timer.step(100 * 256);
        ASSERT_EQ(timer.regDIV(), 102);
        timer.step(153 * 256 + 255);
        ASSERT_EQ(timer.regDIV(), 255);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 0);
    }

    TEST_F(TimerTest, TimerTestDIVPrescaler64) {
        Timer timer;
        timer.setRegTAC(0x06);
        ASSERT_EQ(timer.regDIV(), 0);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 0);
        timer.step(254);
        ASSERT_EQ(timer.regDIV(), 0);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 1);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 1);
        timer.step(255);
        ASSERT_EQ(timer.regDIV(), 2);
        timer.step(100 * 256);
        ASSERT_EQ(timer.regDIV(), 102);
        timer.step(153 * 256 + 255);
        ASSERT_EQ(timer.regDIV(), 255);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 0);
    }

    TEST_F(TimerTest, TimerTestDIVPrescaler256) {
        Timer timer;
        timer.setRegTAC(0x07);
        ASSERT_EQ(timer.regDIV(), 0);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 0);
        timer.step(254);
        ASSERT_EQ(timer.regDIV(), 0);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 1);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 1);
        timer.step(255);
        ASSERT_EQ(timer.regDIV(), 2);
        timer.step(100 * 256);
        ASSERT_EQ(timer.regDIV(), 102);
        timer.step(153 * 256 + 255);
        ASSERT_EQ(timer.regDIV(), 255);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 0);
    }

    TEST_F(TimerTest, TimerTestResetCounterPrescaler16) {
        Timer timer;
        timer.setRegTAC(0x05);
        timer.step(255);
        ASSERT_EQ(timer.regDIV(), 0);
        ASSERT_EQ(timer.regTIMA(), 15);
        timer.setRegDIV(34);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 0);
        ASSERT_EQ(timer.regTIMA(), 15);
        timer.step(254);
        ASSERT_EQ(timer.regDIV(), 0);
        ASSERT_EQ(timer.regTIMA(), 30);
        timer.step(1);
        ASSERT_EQ(timer.regDIV(), 1);
        ASSERT_EQ(timer.regTIMA(), 31);
    }
}