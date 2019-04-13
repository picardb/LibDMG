#include "emulator.hpp"
#include "gtest/gtest.h"

#include <fstream>

using namespace LibDMG;
using namespace std;

namespace {
    // The fixture for testing class Foo.
    class EmulatorTest : public ::testing::Test {
    protected:
        // You can remove any or all of the following functions if its body
        // is empty.

        EmulatorTest() {
            // You can do set-up work for each test here.
        }

        ~EmulatorTest() override {
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

    // Test inexistent cart
    TEST_F(EmulatorTest, EmuRunFor5sOnMachine) {
        Emulator emu;
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 4194304; j++) {
				emu.step(1);
			}
		}

        ofstream out("save_state.xml", ios::binary);
        emu.saveState(out);

        EXPECT_EQ(emu.cpu()->reg16(Cpu::REG16_PC), 100);
    }

	// Test inexistent cart
	TEST_F(EmulatorTest, EmuRunFor5sOnInstr) {
		Emulator emu;
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 1048576; j++) {
				emu.step(4);
			}
		}

		EXPECT_EQ(emu.cpu()->reg16(Cpu::REG16_PC), 100);
	}

    // Test load state
	TEST_F(EmulatorTest, EmuLoadState) {
		Emulator emu;

        ifstream in("save_state.xml", ios::binary);
        emu.loadState(in);

		EXPECT_EQ(emu.cpu()->reg16(Cpu::REG16_PC), 100);
	}
}