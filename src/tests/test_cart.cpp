#include "cart/cart.hpp"
#include "gtest/gtest.h"

using namespace LibDMG;

namespace {
    // The fixture for testing class Foo.
    class CartTest : public ::testing::Test {
    protected:
        // You can remove any or all of the following functions if its body
        // is empty.

        CartTest() {
            // You can do set-up work for each test here.
        }

        ~CartTest() override {
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
    TEST_F(CartTest, CartInvalidFile) {
        ASSERT_THROW(Cart cart("testcart_zeldo.gbc"), CartException);
    }

    // Test Zelda cart
    TEST_F(CartTest, CartZelda) {
        Cart cart("testcart_zelda.gbc");
        EXPECT_EQ(cart.rawMemSize(), 1024 * 1024);
        EXPECT_STREQ(cart.titleStr().c_str(), "ZELDA");
        EXPECT_TRUE(cart.isColorGb());
        EXPECT_TRUE(cart.isSgb());
        EXPECT_EQ(cart.type(), Cart::ROM_MBC5_RAM_BATT);
        EXPECT_STREQ(cart.typeStr().c_str(), "ROM + MBC5 + RAM + BATTERY");
        EXPECT_EQ(cart.romSizeBanks(), 64);
        EXPECT_EQ(cart.romSizeKb(), 1024);
        EXPECT_EQ(cart.ramSizeBanks(), 4);
        EXPECT_EQ(cart.ramSizeKb(), 32);
    }

    // Test Pokemon cart
    TEST_F(CartTest, CartPokemon) {
        Cart cart("testcart_pokemon.gb");
        EXPECT_EQ(cart.rawMemSize(), 1024 * 1024);
        EXPECT_STREQ(cart.titleStr().c_str(), "POKEMON BLUE");
        EXPECT_FALSE(cart.isColorGb());
        EXPECT_TRUE(cart.isSgb());
        EXPECT_EQ(cart.type(), Cart::ROM_MBC3_RAM_BATT);
        EXPECT_STREQ(cart.typeStr().c_str(), "ROM + MBC3 + RAM + BATTERY");
        EXPECT_EQ(cart.romSizeBanks(), 64);
        EXPECT_EQ(cart.romSizeKb(), 1024);
        EXPECT_EQ(cart.ramSizeBanks(), 4);
        EXPECT_EQ(cart.ramSizeKb(), 32);
    }

    // Test BGB cart
    TEST_F(CartTest, CartBgb) {
        Cart cart("testcart_bgb.gb");
        EXPECT_EQ(cart.rawMemSize(), 32 * 1024);
        EXPECT_STREQ(cart.titleStr().c_str(), "BGBWELCOME");
        EXPECT_FALSE(cart.isColorGb());
        EXPECT_FALSE(cart.isSgb());
        EXPECT_EQ(cart.type(), Cart::ROM_ONLY);
        EXPECT_STREQ(cart.typeStr().c_str(), "ROM only");
        EXPECT_EQ(cart.romSizeBanks(), 2);
        EXPECT_EQ(cart.romSizeKb(), 32);
        EXPECT_EQ(cart.ramSizeBanks(), 0);
        EXPECT_EQ(cart.ramSizeKb(), 0);
    }
}