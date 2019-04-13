#include "mem/boot_rom.hpp"
#include "gtest/gtest.h"

using namespace LibDMG;

namespace {
    // The fixture for testing class Foo.
    class BootRomTest : public ::testing::Test {
    protected:
        // You can remove any or all of the following functions if its body
        // is empty.

        BootRomTest() {
            // You can do set-up work for each test here.
        }

        ~BootRomTest() override {
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

    // Test inexistent file
    TEST_F(BootRomTest, BootRomInvalidFile) {
        ASSERT_THROW(BootRom boot("dummy"), BootRomException);
    }

    // Test real file
    TEST_F(BootRomTest, BootRomValidFile) {
        BootRom boot("DMG_ROM.bin");

        ASSERT_EQ(boot.read(0), 0x31);
    }    
}