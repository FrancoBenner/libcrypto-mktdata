#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <chrono>
#include <thread>

#include <gtest/gtest.h>
#include <cloudwall/crypto-mktdata/core.h>

using namespace std::chrono_literals;
using namespace cloudwall::core::marketdata;

TEST(Currency, init) {
    Currency ccy1 = Currency("BTC");
    Currency ccy2 = Currency("ETH");

    ASSERT_EQ("BTC", ccy1.get_ccy_code());
    ASSERT_EQ("ETH", ccy2.get_ccy_code());
    ASSERT_TRUE(ccy1 == ccy1);
    ASSERT_FALSE(ccy1 == ccy2);
}

TEST(ProductId, init) {
    Currency ccy1 = Currency("BTC");
    Currency ccy2 = Currency("ETH");
    Currency ccy3 = Currency("USD");
    CurrencyPair p1 = CurrencyPair(ccy1, ccy3);
    CurrencyPair p2 = CurrencyPair(ccy2, ccy3);

    ASSERT_EQ("BTC", p1.get_quote_ccy().get_ccy_code());
    ASSERT_EQ("ETH", p2.get_quote_ccy().get_ccy_code());
    ASSERT_EQ("USD", p1.get_base_ccy().get_ccy_code());
    ASSERT_EQ("USD", p2.get_base_ccy().get_ccy_code());
    ASSERT_TRUE(p1 == p1);
    ASSERT_FALSE(p1 == p2);
}